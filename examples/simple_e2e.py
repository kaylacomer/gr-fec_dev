#!/usr/bin/env python3

import gnuradio
from gnuradio import gr
from gnuradio import blocks
from gnuradio import fec
from gnuradio import fec_dev
from gnuradio import digital
import numpy as np

import sys
import time


class simple_fg(gr.top_block):
    def __init__(self):
        gr.top_block.__init__(self, "simple encoder/decoder example", catch_exceptions=True)

        samp_rate = 32e3
        puncpat = '11'
        # vector = list(bytes.fromhex('0000000000000000'))
        vector = list(bytes.fromhex('4848500301164607407819311081044c23cb52000000'))
        frame_size = len(vector)
        frame_bits = frame_size * 8

        self.source = blocks.vector_source_b(vector, False, 1, [])
        self.throttle = blocks.throttle( gr.sizeof_char*1, samp_rate, True, 0 if "auto" == "auto" else max( int(float(0.1) * samp_rate) if "auto" == "time" else int(0.1), 1) )
        self.unpack = blocks.unpack_k_bits_bb(8)
        self.enc_ra = enc_ra = fec_dev.bch_encoder.make(frame_bits)
        self.fec_encoder = fec.encoder(enc_ra, gr.sizeof_char, gr.sizeof_char)
        constellation = digital.constellation_bpsk()
        self.mapper = digital.constellation_encoder_bc(constellation)
        self.dec_ra = dec_ra = fec_dev.bch_decoder.make(frame_bits)
        self.fec_decoder = fec.decoder(dec_ra, gr.sizeof_float, gr.sizeof_char)

        self.complex_to_real = blocks.complex_to_real(1)

        self.src_b = blocks.vector_sink_b()
        self.enc_b = blocks.vector_sink_b()
        self.dec_b = blocks.vector_sink_b()
        self.null = blocks.null_sink(gr.sizeof_int)

        self.connect((self.source, 0),(self.throttle,0))
        self.connect((self.throttle, 0),(self.unpack, 0))
        self.connect((self.unpack, 0), (self.fec_encoder, 0))
        self.connect((self.fec_encoder), (self.mapper, 0))
        self.connect((self.mapper,0), (self.complex_to_real,0))
        self.connect((self.complex_to_real, 0), (self.fec_decoder, 0))

        self.connect((self.unpack, 0),(self.src_b, 0))
        self.connect((self.fec_encoder, 0),(self.enc_b, 0))
        self.connect((self.fec_decoder, 0),(self.dec_b, 0))

def main():
    fg = simple_fg()
    fg.start()
    fg.wait()
    src_data = np.array(fg.src_b.data())
    enc_data = np.array(fg.enc_b.data())
    dec_data = np.array(fg.dec_b.data())

    print('unpacked')
    print(f'src: {src_data}')
    print(len(src_data))
    print(f'enc: {enc_data}')
    print(len(enc_data))
    print(f'dec: {dec_data}')
    print(len(dec_data))

    print('\npacked')
    print(f'src: {np.packbits(src_data)}')
    print(f'enc: {np.packbits(enc_data)}')
    print(f'dec: {np.packbits(dec_data)}')

    return True

if __name__ == "__main__":
    sys.exit(not main())



