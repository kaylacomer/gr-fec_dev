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
    def __init__(self, vector, frame_bytes, sigma):
        gr.top_block.__init__(self, "FEC testbench", catch_exceptions=True)

        samp_rate = 32e3
        puncpat = '11'
        
        frame_bits = frame_bytes * BITS_PER_BYTE

        self.source = blocks.vector_source_b(vector, False, 1, [])
        self.throttle = blocks.throttle(gr.sizeof_char, samp_rate, True, 0 if "auto" == "auto" else max( int(float(0.1) * samp_rate) if "auto" == "time" else int(0.1), 1) )
        self.unpack = blocks.unpack_k_bits_bb(BITS_PER_BYTE)
        self.enc_turbo = enc_turbo = fec_dev.turbo_encoder.make(frame_bits)
        self.fec_encoder = fec.encoder(enc_turbo, gr.sizeof_char, gr.sizeof_char)
        constellation = digital.constellation_bpsk()
        constellation.set_npwr(2 * sigma**2)
        self.mapper = digital.constellation_encoder_bc(constellation)
        self.noise = analog.noise_source_c(analog.GR_GAUSSIAN, sigma)
        self.add = blocks.add_vcc(1)
        self.dec_turbo = dec_turbo = fec_dev.turbo_decoder.make(frame_bits)
        self.fec_decoder = fec.decoder(dec_turbo, gr.sizeof_float, gr.sizeof_char)

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

def main(frame_bytes, num_frames, frame_error_count, ebn0):
    bytes_per_vector = frame_bytes * num_frames

    print('Eb/N0 (dB)\tEs/N0 (dB)\tSigma\tFrames\tBE\tBER\t\tFE\tFER\t\tTime elapsed (HH:MM:SS)')
    # TODO useful filename
    filename = 'data.csv'
    with open(filename, 'w') as csvfile:
        csvwriter = csv.writer(csvfile)
        # TODO write simulation info in comments at beginning of file
        csvwriter.writerow(['Eb/N0 (dB)', 'Es/N0 (dB)', 'Sigma', 'Frames', 'Bit errors', 'BER', 'Frame errors', 'FER', 'Time elapsed (HH:MM:SS)'])

        for snr in ebn0:
            sigma = A * 1/np.sqrt(2) * 1/(10**(snr/20))

            # TODO get rate from flowgraph
            rate = 1/3
            bps = 1 # bits per symbol
            esn0 = snr + 10*np.log10(rate * bps)

            bit_errors = frame_errors = loops = 0
            start = timeit.default_timer()

            while (frame_errors < frame_error_count):
                random_generator = np.random.default_rng()
                vector = list(random_generator.bytes(bytes_per_vector * BITS_PER_BYTE))

                fg = test_fg(vector, frame_bytes, sigma)
                fg.start()
                fg.wait()

                src_data = np.array(fg.src_b.data())
                dec_data = np.array(fg.dec_b.data())

                diff = src_data ^ dec_data
                bit_errors += np.count_nonzero(diff)

                diffByFrame = diff.reshape((num_frames, frame_bytes*BITS_PER_BYTE*BITS_PER_BYTE))
                frame_errors += np.count_nonzero([np.count_nonzero(diff) for diff in diffByFrame])

                loops += 1

                frames = loops * num_frames
                bits = frames * frame_bytes*BITS_PER_BYTE*BITS_PER_BYTE
                ber = bit_errors / bits
                fer = frame_errors / frames

                now = timeit.default_timer()
                elapsed_time = now - start
                formatted_time = time.strftime('%H:%M:%S', time.gmtime(elapsed_time))

                print(f'{snr}\t\t{esn0:.2f}\t\t{sigma:.2f}\t{frames}\t{bit_errors}\t{ber:.3E}\t{frame_errors}\t{fer:.3E}\t{formatted_time}', end='\r')
            
            # TODO -- do not let run more than X frames / minutes, set by user
            # TODO calculate throughput = bits tx/second
            print()
            csvwriter.writerow([snr, esn0, sigma, frames, bit_errors, ber, frame_errors, fer, formatted_time])

    return True

if __name__ == "__main__":
    # A = 1.75
    # A = 1
    A = 2

    parser = argparse.ArgumentParser(description='') # TODO add description

    parser.add_argument('-c', '--codec', choices=['turbo'], default='turbo',
                        help='encoding and decoding scheme')

    parser.add_argument('-b', '--frame_bytes', type=int, default=20,
                        help='number of bytes within one frame')
    parser.add_argument('-f', '--num_frames', type=int, default=50,
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
        ebn0 = np.round(np.arange(args.min_ebn0, args.max_ebn0, args.snr_step), int(np.log10(1/args.snr_step)))

    codec = args.codec
    
    # TODO catch keyboard interrupt
    # TODO BCH
    sys.exit(not main(args.frame_bytes, args.num_frames, args.frame_error_count, ebn0))



