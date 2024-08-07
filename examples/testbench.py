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
import csv
import argparse

BITS_PER_BYTE = 8

class test_fg(gr.top_block):
    def __init__(self, vector, frame_bytes, ebn0, codec, t_errors):
        gr.top_block.__init__(self, "FEC testbench", catch_exceptions=True)

        samp_rate = 32e3
        puncpat = '11'
        
        frame_bits = frame_bytes * BITS_PER_BYTE

        self.source = blocks.vector_source_b(vector, False, 1, [])
        self.throttle = blocks.throttle(gr.sizeof_char, samp_rate, True, 0 if "auto" == "auto" else max( int(float(0.1) * samp_rate) if "auto" == "time" else int(0.1), 1) )
        self.unpack = blocks.unpack_k_bits_bb(BITS_PER_BYTE)

        if (codec == 'turbo'):
            self.enc = enc = fec_dev.turbo_encoder.make(frame_bits)
            self.dec = dec = fec_dev.turbo_decoder.make(frame_bits)
        elif (codec == 'BCH'):
            self.enc = enc = fec_dev.bch_encoder.make(frame_bits, t_errors)
            self.dec = dec = fec_dev.bch_decoder.make(frame_bits, t_errors)
        elif (codec == 'RA'):
            self.enc = enc = fec_dev.ra_encoder.make(frame_bits)
            self.dec = dec = fec_dev.ra_decoder.make(frame_bits)
        # TODO error else

        rate = enc.rate()
        bps = 1 # bits per symbol
        self.esn0 = esn0 = ebn0 + 10*np.log10(rate * bps)
        self.sigma = sigma = 1/np.sqrt(2) * 1/np.sqrt((10**(esn0/10)))
        
        self.fec_encoder = fec.encoder(enc, gr.sizeof_char, gr.sizeof_char)
        constellation = digital.constellation_bpsk()
        constellation.set_npwr(2 * sigma**2)
        self.mapper = digital.constellation_encoder_bc(constellation)
        self.noise = analog.noise_source_c(analog.GR_GAUSSIAN, sigma)
        self.add = blocks.add_vcc(1)
        self.fec_decoder = fec.decoder(dec, gr.sizeof_float, gr.sizeof_char)

        self.complex_to_real = blocks.complex_to_real(1)

        self.src_b = blocks.vector_sink_b()
        # self.enc_b = blocks.vector_sink_f()
        self.dec_b = blocks.vector_sink_b()
        # self.null = blocks.null_sink(gr.sizeof_int)

        self.connect((self.source, 0),(self.throttle,0))
        self.connect((self.throttle, 0),(self.unpack, 0))
        self.connect((self.unpack, 0), (self.fec_encoder, 0))
        self.connect((self.fec_encoder), (self.mapper, 0))

        self.connect((self.mapper,0), (self.add,0))
        self.connect((self.noise,0), (self.add,1))
        self.connect((self.add,0), (self.complex_to_real,0))
        self.connect((self.complex_to_real,0), (self.fec_decoder,0))

        self.connect((self.unpack, 0),(self.src_b, 0))
        # self.connect((self.complex_to_real, 0),(self.enc_b, 0))
        self.connect((self.fec_decoder, 0),(self.dec_b, 0))

def main(frame_bytes, num_frames, frame_error_count, ebn0, codec, t_errors):
    bytes_per_vector = frame_bytes * num_frames
    bits_per_vector = bytes_per_vector * BITS_PER_BYTE

    print('Eb/N0 (dB)\tEs/N0 (dB)\tSigma\tFrames\tBE\tBER\t\tFE\tFER\t\tTime elapsed\tThroughput (Mb/s)')
    # TODO useful filename
    filename = 'data.csv'
    with open(filename, 'w') as csvfile:
        csvwriter = csv.writer(csvfile)
        # TODO write simulation info in comments at beginning of file
        csvwriter.writerow(['Eb/N0 (dB)', 'Es/N0 (dB)', 'sigma', 'Frames', 'Bit errors', 'BER', 'Frame errors', 'FER', 'Time elapsed (HH:MM:SS)', 'Throughput (Mb/s)'])

        for snr in ebn0:
            bit_errors = frame_errors = loops = 0
            start = timeit.default_timer()

            while (frame_errors < frame_error_count):
                random_generator = np.random.default_rng()
                vector = list(random_generator.bytes(bytes_per_vector))

                fg = test_fg(vector, frame_bytes, snr, codec, t_errors)
                fg.start()
                fg.wait()

                src_data = np.array(fg.src_b.data())
                dec_data = np.array(fg.dec_b.data())

                diff = src_data ^ dec_data
                bit_errors += np.count_nonzero(diff)


                N_cw = fg.enc.get_output_size()
                N_frames = round(bits_per_vector / N_cw)

                # src_frames = np.reshape(np.packbits(src_data), (N_frames, N_cw))
                # dec_frames = np.reshape(np.packbits(dec_data), (N_frames, N_cw))
                src_frames = np.reshape(np.packbits(src_data), (num_frames, frame_bytes))
                dec_frames = np.reshape(np.packbits(dec_data), (num_frames, frame_bytes))

                for i in range(0, len(src_frames)):
                    if not np.array_equal(src_frames[i], dec_frames[i]):
                        frame_errors += 1

                loops += 1
                
                total_frames = N_frames * loops
                total_bits = bits_per_vector * loops


                ber = bit_errors / total_bits
                fer = frame_errors / total_frames

                now = timeit.default_timer()
                elapsed_time = now - start
                formatted_time = time.strftime('%H:%M:%S', time.gmtime(elapsed_time))
                throughput = total_bits/elapsed_time / 1E6

                sigma = fg.sigma
                esn0 = fg.esn0
                
                print(f'{snr}\t\t{esn0:.2f}\t\t{sigma:.2f}\t{total_frames}\t{bit_errors}\t{ber:.3E}\t{frame_errors}\t{fer:.3E}\t{formatted_time}\t{throughput:.2f}', end='\r')
            
            # TODO -- do not let run more than X frames / minutes, set by user
            # TODO calculate throughput = bits tx/second
            print()
            bits_per_vector = total_frames * frame_bytes*BITS_PER_BYTE*BITS_PER_BYTE
            csvwriter.writerow([snr, esn0, sigma, total_frames, bit_errors, ber, frame_errors, fer, formatted_time, throughput])

    return True

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='') # TODO add description

    parser.add_argument('-c', '--codec', choices=['turbo', 'BCH', 'RA'], default='turbo',
                        help='encoding and decoding scheme')
    parser.add_argument('-t', '--correctable_errors', type=int, default=0,
                        help='if applicable for type of codec (BCH, RS), number of errors that can be corrected')

    parser.add_argument('-b', '--frame_bytes', type=int, default=20,
                        help='number of bytes within one frame')
    parser.add_argument('-f', '--num_frames', type=int, default=1000,
                        help='number of frames in each randomized vector')
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

    args = parser.parse_args()

    if args.ebn0_list:
        ebn0 = args.ebn0_list
    else:
        ebn0 = np.round(np.arange(args.min_ebn0, args.max_ebn0 + args.snr_step, args.snr_step), int(np.log10(1/args.snr_step)))

    codec = args.codec
    t_errors = args.correctable_errors
    # TODO CCSDS supported frame sizes are 1784, 3568, 7136, 8920. Padding with zeros is inefficient
    
    # TODO BCH
    sys.exit(not main(args.frame_bytes, args.num_frames, args.frame_error_count, ebn0, codec, t_errors))



