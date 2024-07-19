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
import time
import random


class test_fg(gr.top_block):
    def __init__(self, vector):
        gr.top_block.__init__(self, "FEC testbench", catch_exceptions=True)

        samp_rate = 32e3
        puncpat = '11'
        
        frame_size = 22
        bits = 8
        frame_bits = frame_size * 8
        sigma = 1.5

        self.source = blocks.vector_source_b(vector, False, 1, [])
        self.throttle = blocks.throttle(gr.sizeof_char*1, samp_rate, True, 0 if "auto" == "auto" else max( int(float(0.1) * samp_rate) if "auto" == "time" else int(0.1), 1) )
        self.unpack = blocks.unpack_k_bits_bb(bits)
        self.enc_turbo = enc_turbo = fec_dev.turbo_encoder.make(frame_bits)
        self.fec_encoder = fec.encoder(enc_turbo, gr.sizeof_char, gr.sizeof_char)
        constellation = digital.constellation_bpsk()
        self.mapper = digital.constellation_encoder_bc(constellation)
        self.noise = analog.noise_source_c(analog.GR_GAUSSIAN, sigma, seed)
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
    numBytes = 176
    random_generator = np.random.default_rng(seed=seed)
    bytes = random_generator.bytes(numBytes)

    vector = list(bytes)
    
    fg = test_fg(vector)
    fg.start()
    fg.wait()

    src_data = np.array(fg.src_b.data())
    enc_data = np.array(fg.enc_b.data())
    dec_data = np.array(fg.dec_b.data())

    print(np.packbits(src_data))
    print(enc_data)
    print(dec_data)
    print(len(src_data))
    print(len(enc_data))
    print(len(dec_data))
    diff = src_data ^ dec_data
    errors = np.count_nonzero(diff)

    print(errors)

    return True

if __name__ == "__main__":
    seed = 10
    sys.exit(not main())



