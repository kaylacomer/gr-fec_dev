#!/usr/bin/env python3

import gnuradio
from gnuradio import gr
from gnuradio import blocks
from gnuradio import fec
from gnuradio import fec_dev
from gnuradio import digital
from gnuradio import analog
from gnuradio.fec import polar
import numpy as np
import sys
import timeit
import time
import datetime
import csv
import argparse

BITS_PER_BYTE = 8
CCSDS_FRAME_SIZES_BITS = [1784, 3568, 7136, 8920]

class test_fg(gr.top_block):
    def __init__(self, vector, K_info, ebn0, codec, t_errors, interleaver, enc_standard, dec_impl, N_cw):
        gr.top_block.__init__(self, "FEC testbench", catch_exceptions=True)

        samp_rate = 32e3

        self.source = blocks.vector_source_b(vector, False, 1, [])
        self.throttle = blocks.throttle(gr.sizeof_char, samp_rate, True, 0 if "auto" == "auto" else max( int(float(0.1) * samp_rate) if "auto" == "time" else int(0.1), 1) )
        self.unpack = blocks.unpack_k_bits_bb(BITS_PER_BYTE)
        
        if (codec == 'Turbo'):
            if (enc_standard == 'LTE'):
                standard = fec_dev.Turbo.LTE
            elif (enc_standard == 'CCSDS'):
                standard = fec_dev.Turbo.CCSDS
            elif (enc_standard == 'CUSTOM'):
                standard = fec_dev.Turbo.CUSTOM
            else:
                print('enc_standard must be LTE/CCSDS/CUSTOM')
                sys.exit(1)
            
            self.enc = enc = fec_dev.turbo_encoder.make(K_info, standard=standard)
            self.dec = dec = fec_dev.turbo_decoder.make(K_info, standard=standard)
        
        elif (codec == 'BCH'):
            self.enc = enc = fec_dev.bch_encoder.make(K_info, t_errors)
            self.dec = dec = fec_dev.bch_decoder.make(K_info, t_errors)
        elif (codec == 'RA'):
            self.enc = enc = fec_dev.ra_encoder.make(K_info)
            self.dec = dec = fec_dev.ra_decoder.make(K_info, iter=10)
        elif (codec == 'RSC'):
            self.enc = enc = fec_dev.rsc_encoder.make(K_info)
            self.dec = dec = fec_dev.rsc_decoder.make(K_info, dec_type=fec_dev.RSC.Viterbi_SIHO)
        elif (codec == 'Polar_AFF3CT'):
            rate = K_info / N_cw
            esn0, sigma = calc_sigma(rate, ebn0)
            self.enc = enc = fec_dev.polar_encoder_aff3ct.make(K_info, N_cw, sigma)
            self.dec = dec = fec_dev.polar_decoder_aff3ct.make(K_info, N_cw, sigma)
        elif (codec == 'TPC_AFF3CT'):
            rate = K_info / N_cw
            esn0, sigma = calc_sigma(rate, ebn0)
            self.enc = enc = fec_dev.tpc_encoder_aff3ct.make(K_info, N_cw, t_errors)
            self.dec = dec = fec_dev.tpc_decoder_aff3ct.make(K_info, N_cw, t_errors)
        elif (codec == 'Polar'):
            mu = 10 # number of intervals, Tal and Vardy
            # rate = K_info / N_cw
            # esn0, sigma = calc_sigma(rate, ebn0)
            
            polar_config = fec.polar.load_frozen_bits_info(False, fec.polar.CHANNEL_TYPE_BEC, N_cw, K_info, ebn0, mu)
            # polar_config = fec.polar.tal_vardy_tpm_algorithm(N_cw, ebn0, mu)
            self.enc = enc = fec.polar_encoder.make(N_cw, K_info, polar_config['positions'], polar_config['values'], False)
            self.dec = dec = fec.polar_decoder_sc.make(N_cw, K_info, polar_config['positions'], polar_config['values'])

        else:
            print('codec chosen {} not available in testbench'.format(codec))
            sys.exit(1)

        
        self.fec_encoder = fec.encoder(enc, gr.sizeof_char, gr.sizeof_char)
        self.fec_decoder = fec.decoder(dec, gr.sizeof_float, gr.sizeof_char)

        # self.puncpat = puncpat ='11'
        # self.threading = None
        # self.ext_encoder = fec.extended_encoder(
        #     enc, threading=self.threading, puncpat=self.puncpat)
        # self.ext_decoder = fec.extended_decoder(dec, threading=self.threading, ann=None,
        #                                     puncpat=self.puncpat, integration_period=10000)

        rate = dec.rate()
        self.esn0, self.sigma = calc_sigma(rate, ebn0)
        # bps = 1 # bits per symbol
        # self.esn0 = esn0 = ebn0 + 10*np.log10(rate * bps)
        # self.sigma = sigma = 1/np.sqrt(2) * 1/np.sqrt((10**(esn0/10)))

        constellation = digital.constellation_bpsk()
        constellation.set_npwr(2 * self.sigma**2)
        self.mapper = digital.constellation_encoder_bc(constellation)
        self.complex_to_real = blocks.complex_to_real(1)

        self.add = blocks.add_vff(1)
        self.noise = analog.noise_source_f(analog.GR_GAUSSIAN, self.sigma)

        self.src_b = blocks.vector_sink_b()
        self.enc_b = blocks.vector_sink_b()
        self.dec_b = blocks.vector_sink_b()

        self.connect((self.source, 0),(self.throttle,0))
        self.connect((self.throttle, 0),(self.unpack, 0))
        self.connect((self.unpack, 0), (self.fec_encoder, 0))

        self.connect((self.fec_encoder), (self.mapper, 0))

        self.connect((self.mapper,0), (self.complex_to_real,0))
        self.connect((self.complex_to_real,0), (self.add,0))
        self.connect((self.noise,0), (self.add,1))
        self.connect((self.add,0), (self.fec_decoder,0))

        self.connect((self.fec_decoder, 0),(self.dec_b, 0))
        self.connect((self.fec_encoder, 0),(self.enc_b, 0))

        self.connect((self.unpack, 0),(self.src_b, 0))

def calc_sigma(rate, ebn0):
    bps = 1 # bits per symbol
    esn0 = ebn0 + 10*np.log10(rate * bps)
    sigma = 1/np.sqrt(2) * 1/np.sqrt((10**(esn0/10)))

    return esn0, sigma

def file_info(encoder, decoder, codec, K_info, ebn0, now):
    N_cw = encoder.get_output_size()
    rate = decoder.rate()
    # K = encoder.get_input_size()
    # zeros = K - K_info
    # N_cw = N + zeros
    
    info = [
        ['[metadata]'],
        [f'title=GR {codec} ({N_cw}N,{K_info}K)'],
        [''],
        ['[trace]'],
        ['# ----------------------------------------------------'],
        ['# --GNU Radio FEC Testbench, based on AFF3CT library--'],
        ['# ----------------------------------------------------'],
        ['# Parameters :'],
        ['# * Simulation -------------------------------------'],
        ['#    ** Type                   = GNU Radio'],
        ['#    ** Type of bits           = int8'],
        ['#    ** Type of reals          = float32'],
        ['#    ** Type of quant. reals   = int8'],
        ['#    ** Date (UTC)             = {}'.format(now)],
        ['#    ** Git commit ID          = '],
        ['#    ** Code type (C)          = {}'.format(codec)],
        ['#    ** Noise range            = {} -> {} dB'.format(min(ebn0), max(ebn0))],
        ['#    ** Noise type             = EBN0'],
        ['#    ** Bit rate               = {:.6f} ({}/{})'.format(rate, K_info, N_cw)],
        ['# * Source ----------------------------------------'],
        ['#    ** Type                   = Random'],
        ['#    ** Implementation         = Numpy Generator'],
        ['#    ** Info. bits (K_info)    = {}'.format(K_info)],
        ['# * Codec -----------------------------------------'],
        ['#    ** Type                   = {}'.format(codec)],
        ['#    ** Info. bits (K_info)    = {}'.format(K_info)],
        ['#    ** Codeword size (N_cw)   = {}'.format(N_cw)],
        # ['#    ** Frame size (N)         = {}'.format(N_cw)],
        ['#    ** Code rate              = {:.6f} ({}/{})'.format(rate, K_info, N_cw)],
        # ['# * Encoder ---------------------------------------'],
        # ['#    ** Type                   = {}'.format(codec)],
        # ['#    ** Systematic             = yes'], #start TODO
        # ['#    ** Tail length            = 12'], #TODO
        # ['#    Encoder --------------------------------------'],
        # ['#    ** Type                   = RSC'], #TODO
        # ['#    ** Systematic             = yes'], #TODO
        # ['#    ** Tail length            = 6'],
        # ['#    ** Buffered               = on'],
        # ['#    ** Standard               = LTE'],
        # ['#    ** Polynomials            = {013,015}'],
        # ['# * Interleaver -----------------------------------'],
        # ['#    ** Type                   = LTE'],
        # ['#    ** Size                   = 6144'],
        # ['# * Decoder ---------------------------------------'],
        # ['#    ** Type (D)               = {}'.format(codec)],
        # ['#    ** Implementation         = FAST'],
        # ['#    ** Systematic             = yes'],
        # ['#    ** Num. of iterations (i) = 6'],
        # ['#    Decoder --------------------------------------'],
        # ['#    ** Type (D)               = BCJR'],
        # ['#    ** Implementation         = VERY_FAST'],
        # ['#    ** Systematic             = yes'],
        # ['#    ** Standard               = LTE'],
        # ['#    ** Polynomials            = {013,015}'], #end TODO
        # ['# * Modem -----------------------------------------'],
        # ['#    ** Type                   = BPSK'],
        # ['#    ** Implementation         = GNU Radio constellation encoder'],
        # ['#    ** Bits per symbol        = 1'],
        # ['# * Channel ---------------------------------------'],
        # ['#    ** Type                   = AWGN'],
        # ['#    ** Implementation         = GNU Radio noise source'],
        # ['#    ** Complex                = off'],
        # ['# * Quantizer -------------------------------------'],
        # ['#    ** Type                   = POW2'], # TODO quantizer
        # ['#    ** Implementation         = STD'],
        # ['#    ** Fixed-point config.    = {6, 2}'],
        # ['#'],
        ['# GNU Radio simulation is running...'],
        ['# --------------------------------||----------------------------------------------------------||---------------------'],
        ['#        Signal Noise Ratio       ||     Bit Error Rate (BER) and Frame Error Rate (FER)      ||  Global throughput'],
        ['#              (SNR)              ||                                                          ||  and elapsed time'],
        ['# --------------------------------||----------------------------------------------------------||---------------------'],
        ['# ----------|----------|----------||------------|----------|----------|-----------|-----------||----------|----------'],
        ['#     Sigma |    Es/N0 |    Eb/N0 ||        FRA |       BE |       FE |       BER |       FER ||  SIM_THR |    ET/RT'],
        ['#           |     (dB) |     (dB) ||            |          |          |           |           ||   (Mb/s) | (hhmmss) '],
        ['# ----------|----------|----------||------------|----------|----------|-----------|-----------||----------|----------'],
    ]

    for item in info:
        for inner in item:
            print(inner)

    return info


def main(K_info, num_frames, frame_error_count, ebn0, codec, t_errors, max_seconds, max_frames, interleaver, enc_standard, dec_impl, N_cw):
    bits_per_vector = K_info * num_frames
    bytes_per_vector = bits_per_vector / BITS_PER_BYTE

    now = datetime.datetime.now(datetime.timezone.utc).strftime("%Y-%m-%d %H-%M-%S")
    filename = f'gr-fec-{codec}_K{K_info}_{interleaver}-{now}.txt'

    file_header_written = False
    with open(filename, 'w') as txtfile:
        txtwriter = csv.writer(txtfile)

        for snr in ebn0:
            bit_errors = frame_errors = loops = 0
            start = timeit.default_timer()

            abandon = False
            while (frame_errors < frame_error_count) and not abandon:
                random_generator = np.random.default_rng()
                vector = list(random_generator.bytes(bytes_per_vector))

                fg = test_fg(vector, K_info, snr, codec, t_errors, interleaver, enc_standard, dec_impl, N_cw)
                fg.start()
                fg.wait()

                if not file_header_written:
                    info = file_info(fg.enc, fg.dec, codec, K_info, ebn0, now)
                    txtwriter.writerows(info)
                    file_header_written = True

                src_data = np.array(fg.src_b.data())
                # enc_data = np.array(fg.enc_b.data())
                dec_data = np.array(fg.dec_b.data())

                # print(f'len(src_data): {len(src_data)}')
                # print(f'len(enc_data): {len(enc_data)}')
                # print(f'len(dec_data): {len(dec_data)}')

                # print(f'src: {np.packbits(src_data)}')
                # print(f'dec: {np.packbits(dec_data)}')

                diff = src_data ^ dec_data
                bit_errors += np.count_nonzero(diff)

                K_info_bytes = int(K_info / BITS_PER_BYTE)
                src_frames = np.reshape(np.packbits(src_data), (num_frames, K_info_bytes))
                dec_frames = np.reshape(np.packbits(dec_data), (num_frames, K_info_bytes))

                for i in range(0, len(src_frames)):
                    if not np.array_equal(src_frames[i], dec_frames[i]):
                        frame_errors += 1
                
                loops += 1
                
                total_frames = num_frames * loops
                total_bits = bits_per_vector * loops

                ber = bit_errors / total_bits
                fer = frame_errors / total_frames

                now = timeit.default_timer()
                elapsed_time = now - start
                formatted_time = time.strftime('%Hh%M\'%S', time.gmtime(elapsed_time))
                throughput = total_bits/elapsed_time / 1E6

                sigma = fg.sigma
                esn0 = fg.esn0
                
                f = '{:11.4f} |{:9.3f} |{:9.3f} ||{:11} |{:9} |{:9} |{:10.3e} |{:10.3e} ||{:9.3f} | {}'.format(sigma, esn0, snr, total_frames, bit_errors, frame_errors, ber, fer, throughput, formatted_time)
                print(f, end='\r')

                abandon = max_seconds > 0 and elapsed_time > max_seconds or max_frames > 0 and total_frames > max_frames
                if abandon:
                    print('end of simulation step due to time or number of frames limit')
            
            print()
            txtwriter.writerow([f])

        end = '# End of the GNU Radio FEC testbench simulation (based on AFF3CT).'
        print(end)
        txtwriter.writerow([end])
        
    return True

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='') # TODO add description

    parser.add_argument('-C', '--codec', choices=['Turbo', 'BCH', 'RA', 'LDPC_AFF3CT', 'Polar_AFF3CT', 'REP', 'RS', 'RSC', 'TPC_AFF3CT', 'Polar'],
                        help='encoding and decoding scheme')

    parser.add_argument('-K', '--info_bits', type=int, default=160,
                        help='number of info bits in one frame, typically same as number of input bits to encoder')
    parser.add_argument('-N', '--codeword_size', type=int, default=0,
                        help='number of bits in encoded frame, typically same as number of output bits from encoder ... for most codecs, calculated automatically')
    parser.add_argument('-F', '--num_frames', type=int, default=1000,
                        help='number of frames in each randomized vector / flowgraph simulation')
    parser.add_argument('-E', '--frame_error_count', type=int, default=100,
                        help='simulation will continue until this number of frame errors is reached')
    
    parser.add_argument('-m', '--min_ebn0', type=float, default=0,
                        help='minimum simulated signal-to-noise ratio (Eb/N0) in dB (energy per bit over noise energy)')
    parser.add_argument('-M', '--max_ebn0', type=float, default=0.5,
                        help='maximum simulated signal-to-noise ratio (Eb/N0) in dB (energy per bit over noise energy)')
    parser.add_argument('-S', '--snr_step', type=float, default=0.1,
                        help='SNR (Eb/N0) step in dB between each simulation iteration')
    
    parser.add_argument('-L', '--ebn0_list', nargs='*',
                        help='list of SNRs (Eb/N0 in dB) to simulate; if set, ignores minimum, maximum, and step parameters')
    
    parser.add_argument('-t', '--max_seconds', type=int, default=0,
                        help='maximum time simulation runs in seconds ... if set to 0, no limit')
    parser.add_argument('-f', '--max_frames', type=int, default=0,
                        help='maximum number of frames simulated ... if set to 0, no limit')
    
    parser.add_argument('-I', '--interleaver', choices=['NO', 'COL_ROW', 'ROW_COL', 'RANDOM', 'RAND_COL',
                                'GOLDEN', 'CCSDS', 'LTE', 'DVB_RCS1', 'DVB_RCS2'], default='NO',
                        help='if applicable for codec (RA, turbo), interleaver type ... \
                        for turbo, sets additional parameters based on choice (polynomials, trellis size)')
    
    parser.add_argument('-T', '--correctable_errors', type=int, default=0,
                        help='if applicable for codec (BCH, RS), number of errors that can be corrected')
    
    parser.add_argument('--enc_standard', default='LTE', choices=['LTE', 'CCSDS', 'CUSTOM'],
                        help='for Turbo codes, standard to use (LTE, CCSDS, CUSTOM) -- automatically sets some parameters')
    parser.add_argument('--dec_impl', default='STD', choices=['STD', 'FAST', 'VERY_FAST', 'GENIUS', 'NAIVE'],
                        help='decoder implementation, available for some codecs')

    args = parser.parse_args()

    num_frames = args.num_frames
    K_info = args.info_bits
    N_cw = args.codeword_size
    codec = args.codec
    t_errors = args.correctable_errors
    interleaver = args.interleaver
    max_seconds = args.max_seconds
    max_frames = args.max_frames
    frame_error_count = args.frame_error_count
    enc_standard = args.enc_standard
    dec_impl = args.dec_impl

    # if num_frames % 4:
    #     print('Number of frames (-f, --num_frames) must be divisible by 4')
    #     sys.exit(1)
    if K_info % BITS_PER_BYTE:
        print(f'K_info (info bits) must be divisible by {BITS_PER_BYTE}')
        sys.exit(1)
    if enc_standard == 'CCSDS' and K_info not in CCSDS_FRAME_SIZES_BITS:
        print(f'CCSDS supported frame sizes are {CCSDS_FRAME_SIZES_BITS}')
        sys.exit(1)

    if args.ebn0_list:
        ebn0 = args.ebn0_list
    else:
        ebn0 = np.round(np.arange(args.min_ebn0, args.max_ebn0 + args.snr_step, args.snr_step), int(np.log10(1/args.snr_step)))
    
    sys.exit(not main(K_info, num_frames, frame_error_count, ebn0, codec, t_errors, max_seconds, max_frames, interleaver, enc_standard, dec_impl, N_cw))



