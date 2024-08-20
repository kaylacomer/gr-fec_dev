#!/usr/bin/env python3

import gnuradio
from gnuradio import gr
from gnuradio import blocks
from gnuradio import fec
from gnuradio import fec_dev
from gnuradio import digital
from gnuradio import analog
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
    def __init__(self, vector, K_info, ebn0, codec, t_errors, interleaver):
        gr.top_block.__init__(self, "FEC testbench", catch_exceptions=True)

        samp_rate = 32e3
        puncpat = '11'

        self.source = blocks.vector_source_b(vector, False, 1, [])
        self.throttle = blocks.throttle(gr.sizeof_char, samp_rate, True, 0 if "auto" == "auto" else max( int(float(0.1) * samp_rate) if "auto" == "time" else int(0.1), 1) )
        self.unpack = blocks.unpack_k_bits_bb(BITS_PER_BYTE)

        if (codec == 'Turbo'):
            if (interleaver == 'LTE'):
                standard = fec_dev.LTE
            elif (interleaver == 'CCSDS'):
                standard = fec_dev.CCSDS
            else:
                print('MUST specify interleaver for turbo codec')
                sys.exit(1)
            
            self.enc = enc = fec_dev.turbo_encoder.make(K_info, standard)
            self.dec = dec = fec_dev.turbo_decoder.make(K_info, standard)
        
        elif (codec == 'BCH'):
            self.enc = enc = fec_dev.bch_encoder.make(K_info, t_errors)
            self.dec = dec = fec_dev.bch_decoder.make(K_info, t_errors)
        elif (codec == 'RA'):
            self.enc = enc = fec_dev.ra_encoder.make(K_info)
            self.dec = dec = fec_dev.ra_decoder.make(K_info)
        # TODO error else
        
        self.fec_encoder = fec.encoder(enc, gr.sizeof_char, gr.sizeof_char)

        rate = enc.rate()
        bps = 1 # bits per symbol
        self.esn0 = esn0 = ebn0 + 10*np.log10(rate * bps)
        self.sigma = sigma = 1/np.sqrt(2) * 1/np.sqrt((10**(esn0/10)))

        constellation = digital.constellation_bpsk()
        constellation.set_npwr(2 * sigma**2)
        self.mapper = digital.constellation_encoder_bc(constellation)
        self.complex_to_real = blocks.complex_to_real(1)

        self.add = blocks.add_vff(1)
        self.noise = analog.noise_source_f(analog.GR_GAUSSIAN, sigma)

        self.fec_decoder = fec.decoder(dec, gr.sizeof_float, gr.sizeof_char)

        self.src_b = blocks.vector_sink_b()
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

        self.connect((self.unpack, 0),(self.src_b, 0))

def file_info(encoder, decoder, codec, K_info, ebn0, now):
    K = encoder.get_input_size()
    N = encoder.get_output_size()
    rate = encoder.rate()
    N_cw = N #TODO
    
    info = [
        ['[metadata]'],
        [f'title=GR {codec} ({N},{K}) BCJR 8-bit 6 ite. {interleaver}'], #TODO
        [''],
        ['[trace]'],
        ['# ----------------------------------------------------'],
        ['# --GNU Radio FEC Testbench, based on AFF3CT library--'],
        ['# ----------------------------------------------------'],
        ['# Parameters :'],
        ['# * Simulation -------------------------------------'],
        ['#    ** Type                   = GNU Radio'],
        ['#    ** Type of bits           = int8'],
        ['#    ** Type of reals          = float32'], #TODO check
        ['#    ** Type of quant. reals   = int8'], # TODO check
        ['#    ** Date (UTC)             = {}'.format(now)],
        ['#    ** Git version            = '],
        ['#    ** Code type (C)          = {}'.format(codec)],
        ['#    ** Noise range            = {} -> {} dB'.format(min(ebn0), max(ebn0))],
        ['#    ** Noise type (E)         = EBN0'],
        ['#    ** Bit rate               = {:.6f} ({}/{})'.format(rate, K, N)],
        ['# * Source ----------------------------------------'],
        ['#    ** Type                   = Random'],
        ['#    ** Implementation         = Numpy Generator'],
        ['#    ** Info. bits (K_info)    = {}'.format(K_info)],
        ['# * Codec -----------------------------------------'],
        ['#    ** Type                   = {}'.format(codec)],
        ['#    ** Info. bits (K_info)         = {}'.format(K_info)],
        ['#    ** Codeword size (N_cw)   = {}'.format(N_cw)],
        ['#    ** Frame size (N)         = {}'.format(N)],
        ['#    ** Code rate              = {:.6f} ({}/{})'.format(rate, K, N)],
        ['# * Encoder ---------------------------------------'],
        ['#    ** Type                   = {}'.format(codec)],
        ['#    ** Systematic             = yes'], #start TODO
        ['#    ** Tail length            = 12'], #TODO
        ['#    Encoder --------------------------------------'],
        ['#    ** Type                   = RSC'], #TODO
        ['#    ** Systematic             = yes'], #TODO
        ['#    ** Tail length            = 6'],
        ['#    ** Buffered               = on'],
        ['#    ** Standard               = LTE'],
        ['#    ** Polynomials            = {013,015}'],
        ['# * Interleaver -----------------------------------'],
        ['#    ** Type                   = LTE'],
        ['#    ** Size                   = 6144'],
        ['# * Decoder ---------------------------------------'],
        ['#    ** Type (D)               = {}'.format(codec)],
        ['#    ** Implementation         = FAST'],
        ['#    ** Systematic             = yes'],
        ['#    ** Num. of iterations (i) = 6'],
        ['#    Decoder --------------------------------------'],
        ['#    ** Type (D)               = BCJR'],
        ['#    ** Implementation         = VERY_FAST'],
        ['#    ** Systematic             = yes'],
        ['#    ** Standard               = LTE'],
        ['#    ** Polynomials            = {013,015}'], #end TODO
        ['# * Modem -----------------------------------------'],
        ['#    ** Type                   = BPSK'],
        ['#    ** Implementation         = GNU Radio constellation encoder'],
        ['#    ** Bits per symbol        = 1'],
        ['# * Channel ---------------------------------------'],
        ['#    ** Type                   = AWGN'],
        ['#    ** Implementation         = GNU Radio noise source'],
        ['#    ** Complex                = off'],
        ['# * Quantizer -------------------------------------'],
        ['#    ** Type                   = POW2'], # TODO quantizer
        ['#    ** Implementation         = STD'],
        ['#    ** Fixed-point config.    = {6, 2}'],
        ['#'],
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
            print()
        print()

    return info


def main(K_info, num_frames, frame_error_count, ebn0, codec, t_errors, max_seconds, max_frames, interleaver):
    bits_per_vector = K_info * num_frames
    bytes_per_vector = bits_per_vector / BITS_PER_BYTE

    now = datetime.datetime.now(datetime.timezone.utc).strftime("%Y-%m-%d %H-%M-%S")
    # filename = f'gr-fec-testbench-{now}.txt'
    filename = f'gr-fec-{codec}_K{K_info}_{interleaver}-{now}.txt'
    # info = [['# '],]
    #         ['# ----- GNU Radio FEC Testbench -----'],
    #         ['# '],
    #         ['# Date:                   ',f'{now}'],
    #         ['# Codec:                  ',f'{codec}'],
    #         ['# Interleaver (turbo/RA): ',f'{interleaver}'],
    #         ['# t (BCH/RS only):        ',f'{t_errors}'],
    #         ['# Frame bits (bits):     ',f'{frame_bytes} ({frame_bytes*BITS_PER_BYTE})'],
    #         ['# Frames per message:     ',f'{num_frames}'],
    #         ['# Frame errors:           ',f'{frame_error_count}'],
    #         ['# Min Eb/N0:              ',f'{min(ebn0)}'],
    #         ['# Max Eb/N0:              ',f'{max(ebn0)}'],
    #         ['# Max simulation sec:     ',f'{max_seconds}'],
    #         ['# Max simulation frames:  ',f'{max_frames}'],
    #         ['# '],
    #         [],
    #         ['Eb/N0 (dB)','Es/N0 (dB)','Sigma','Frames\t','BE','BER','FE','FER','Time elapsed','Throughput (Mb/s)']
    # ]

    # for item in info:
        # for inner in item:
        #     print(f'{inner:15}', end='')
        # print()

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

                fg = test_fg(vector, K_info, snr, codec, t_errors, interleaver)
                fg.start()
                fg.wait()

                if not file_header_written:
                    info = file_info(fg.enc, fg.dec, codec, K_info, ebn0, now)
                    txtwriter.writerows(info)
                    file_header_written = True

                src_data = np.array(fg.src_b.data())
                dec_data = np.array(fg.dec_b.data())

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

    parser.add_argument('-c', '--codec', choices=['Turbo', 'BCH', 'RA'], default='Turbo',
                        help='encoding and decoding scheme')
    parser.add_argument('-i', '--interleaver', choices=['NO', 'COL_ROW', 'ROW_COL', 'RANDOM', 'RAND_COL',
                                'GOLDEN', 'CCSDS', 'LTE', 'DVB_RCS1', 'DVB_RCS2'], default='NO',
                        help='if applicable for codec (RA, turbo), interleaver type ... \
                        for turbo, sets additional parameters based on choice (polynomials, trellis size)')
    parser.add_argument('-t', '--correctable_errors', type=int, default=0,
                        help='if applicable for codec (BCH, RS), number of errors that can be corrected')

    parser.add_argument('-K', '--info_bits', type=int, default=160,
                        help='number of info bits in one frame, typically same as frame bits')
    parser.add_argument('-f', '--num_frames', type=int, default=1000,
                        help='number of frames in each randomized vector ... MUST be divisible by 4')
    parser.add_argument('-e', '--frame_error_count', type=int, default=100,
                        help='simulation will continue until this number of frame errors is reached')
    
    parser.add_argument('-m', '--min_ebn0', type=float, default=0,
                        help='minimum simulated signal-to-noise ratio (Eb/N0) in dB (energy per bit over noise energy)')
    parser.add_argument('-M', '--max_ebn0', type=float, default=0.5,
                        help='maximum simulated signal-to-noise ratio (Eb/N0) in dB (energy per bit over noise energy)')
    parser.add_argument('-s', '--snr_step', type=float, default=0.1,
                        help='SNR (Eb/N0) step in dB between each simulation iteration')
    
    parser.add_argument('-l', '--ebn0_list', nargs='*',
                        help='list of SNRs (Eb/N0 in dB) to simulate; if set, ignores minimum, maximum, and step parameters')
    
    parser.add_argument('-T', '--max_seconds', type=int, default=0,
                        help='maximum time simulation runs in seconds ... if set to 0, no limit')
    parser.add_argument('-F', '--max_frames', type=int, default=0,
                        help='maximum number of frames simulated ... if set to 0, no limit')

    args = parser.parse_args()

    num_frames = args.num_frames
    K_info = args.info_bits
    codec = args.codec
    t_errors = args.correctable_errors
    interleaver = args.interleaver
    max_seconds = args.max_seconds
    max_frames = args.max_frames
    frame_error_count = args.frame_error_count

    # if num_frames % 4:
    #     print('Number of frames (-f, --num_frames) must be divisible by 4')
    #     sys.exit(1)
    if K_info % BITS_PER_BYTE:
        print(f'K_info (info bits) must be divisible by {BITS_PER_BYTE}')
    if interleaver == 'CCSDS' and K_info not in CCSDS_FRAME_SIZES_BITS:
        print(f'CCSDS supported frame sizes are {CCSDS_FRAME_SIZES_BITS}')
        sys.exit(1)

    if args.ebn0_list:
        ebn0 = args.ebn0_list
    else:
        ebn0 = np.round(np.arange(args.min_ebn0, args.max_ebn0 + args.snr_step, args.snr_step), int(np.log10(1/args.snr_step)))
    
    
    # TODO BCH
    sys.exit(not main(K_info, num_frames, frame_error_count, ebn0, codec, t_errors, max_seconds, max_frames, interleaver))



