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
        self.noise = analog.noise_source_c(analog.GR_GAUSSIAN, sigma, SEED)
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
    frame_bytes = 20
    num_frames = 20
    total_bytes = frame_bytes * num_frames

    random_generator = np.random.default_rng(seed=SEED)
    vector = list(random_generator.bytes(total_bytes * BITS_PER_BYTE))
    # msg = [list(random_generator.bytes(frame_bytes * BITS_PER_BYTE)) for n in range(0,num_frames)]

    filename = 'data.csv'
    with open(filename, 'w') as csvfile:
        csvwriter = csv.writer(csvfile)
        # write simulation info in comments at beginning of file
        csvwriter.writerow(['Sigma', 'Bit errors', 'BER', 'Frame errors', 'FER', 'Time elapsed (HH:MM:SS)'])

        for sigma in [1.0, 1.1, 1.2, 1.3, 1.4, 1.5]:
            print(f'sigma: {sigma}')
            fg = test_fg(vector, frame_bytes, sigma)
            fg.start()
            start = timeit.default_timer()
            fg.wait()
            end = timeit.default_timer()
            elapsed_time = end - start

            src_data = np.array(fg.src_b.data())
            # enc_data = np.array(fg.enc_b.data())
            dec_data = np.array(fg.dec_b.data())

            bits = len(src_data)
            diff = src_data ^ dec_data
            bit_errors = np.count_nonzero(diff)
            ber = bit_errors / bits

            diffByFrame = diff.reshape((num_frames, frame_bytes*BITS_PER_BYTE*BITS_PER_BYTE))
            frame_errors = np.count_nonzero([np.count_nonzero(diff) for diff in diffByFrame])
            fer = frame_errors / len(diffByFrame)

            # throughput = bits / elapsed_time / 10 ** 6 ###### not as simple - need bits transmitted or received per second
            formatted_time = time.strftime('%H:%M:%S', time.gmtime(elapsed_time))

            csvwriter.writerow([sigma, bit_errors, ber, frame_errors, fer, formatted_time])

    return True

if __name__ == "__main__":
    BITS_PER_BYTE = 8
    SEED = 25
    
    sys.exit(not main())



