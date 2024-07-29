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
        self.mapper = digital.constellation_encoder_bc(constellation)
        self.noise = analog.noise_source_c(analog.GR_GAUSSIAN, sigma)
        self.add = blocks.add_vcc(1)
        self.dec_turbo = dec_turbo = fec_dev.turbo_decoder.make(frame_bits)
        self.fec_decoder = fec.decoder(dec_turbo, gr.sizeof_float, gr.sizeof_char)

        self.complex_to_real = blocks.complex_to_real(1)

        self.src_b = blocks.vector_sink_b()
        self.enc_b = blocks.vector_sink_f()
        self.dec_b = blocks.vector_sink_b()
        self.null = blocks.null_sink(gr.sizeof_int)

        self.connect((self.source, 0),(self.throttle,0))
        self.connect((self.throttle, 0),(self.unpack, 0))
        self.connect((self.unpack, 0), (self.fec_encoder, 0))
        self.connect((self.fec_encoder), (self.mapper, 0))

        self.connect((self.mapper,0), (self.add,0))
        self.connect((self.noise,0), (self.add,1))
        self.connect((self.add,0), (self.complex_to_real,0))
        self.connect((self.complex_to_real,0), (self.fec_decoder,0))

        self.connect((self.unpack, 0),(self.src_b, 0))
        self.connect((self.complex_to_real, 0),(self.enc_b, 0))
        self.connect((self.fec_decoder, 0),(self.dec_b, 0))

def main():
    total_bytes = frame_bytes * num_frames

    filename = 'data.csv'
    with open(filename, 'w') as csvfile:
        csvwriter = csv.writer(csvfile)
        # TODO write simulation info in comments at beginning of file
        csvwriter.writerow(['Eb/N0 (dB)', 'Es/N0 (dB)', 'Sigma', 'Bit errors', 'BER', 'Frame errors', 'FER', 'Time elapsed (HH:MM:SS)'])

        ebn0 = [10, 10.1, 10.2, 10.3, 10.4, 10.5, 10.6] # dB

        for snr in ebn0:
            sigma = 1/np.sqrt(2) * 1/(10**(snr/20))
            sigma = 1.5

            # TODO get rate from flowgraph
            rate = 1/3
            bps = 1 # bits per symbol
            esn0 = snr + 10*np.log10(rate * bps)

            bit_errors = frame_errors = loops = 0
            start = timeit.default_timer()

            while (frame_errors < frame_error_count):
                random_generator = np.random.default_rng()
                vector = list(random_generator.bytes(total_bytes * BITS_PER_BYTE))

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
            
            end = timeit.default_timer()
            elapsed_time = end - start
            formatted_time = time.strftime('%H:%M:%S', time.gmtime(elapsed_time))
            # TODO -- do not let run more than X frames / minutes, set by user
            # TODO calculate throughput = bits tx/second

            bits = loops * num_frames * frame_bytes*BITS_PER_BYTE*BITS_PER_BYTE
            ber = bit_errors / bits
            fer = frame_errors / (loops * num_frames)

            csvwriter.writerow([snr, esn0, sigma, bit_errors, ber, frame_errors, fer, formatted_time])

    return True

if __name__ == "__main__":
    BITS_PER_BYTE = 8

    frame_bytes = 20
    num_frames = 20
    frame_error_count = 100

    # parser = argparse.ArgumentParser(description='Simulate .')
    # parser.add_argument('frame_bytes', metavar='N', type=int, nargs='+',
    #                     help='an integer for the accumulator')
    # parser.add_argument('--sum', dest='accumulate', action='store_const',
    #                     const=sum, default=max,
    #                     help='sum the integers (default: find the max)')

    # args = parser.parse_args()
    # print(args.accumulate(args.integers))

    
    # TODO command line arguments
    # TODO command line print updates like aff3ct (if simple to code)
    sys.exit(not main())



