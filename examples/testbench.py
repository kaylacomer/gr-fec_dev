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
CCSDS_FRAME_SIZES = [223, 446, 892, 1115]

class test_fg(gr.top_block):
    def __init__(self, vector, frame_bytes, ebn0, codec, t_errors, interleaver):
        gr.top_block.__init__(self, "FEC testbench", catch_exceptions=True)

        samp_rate = 32e3
        puncpat = '11'
        
        frame_bits = frame_bytes * BITS_PER_BYTE

        self.source = blocks.vector_source_b(vector, False, 1, [])
        self.throttle = blocks.throttle(gr.sizeof_char, samp_rate, True, 0 if "auto" == "auto" else max( int(float(0.1) * samp_rate) if "auto" == "time" else int(0.1), 1) )
        self.unpack = blocks.unpack_k_bits_bb(BITS_PER_BYTE)

        if (codec == 'turbo'):
            if (interleaver == 'LTE'):
                standard = fec_dev.LTE
            elif (interleaver == 'CCSDS'):
                standard = fec_dev.CCSDS
            else:
                print('MUST specify interleaver for turbo codec')
                sys.exit(1)
            
            self.enc = enc = fec_dev.turbo_encoder.make(frame_bits, standard)
            self.dec = dec = fec_dev.turbo_decoder.make(frame_bits, standard)
        
        elif (codec == 'BCH'):
            self.enc = enc = fec_dev.bch_encoder.make(frame_bits, t_errors)
            self.dec = dec = fec_dev.bch_decoder.make(frame_bits, t_errors)
        elif (codec == 'RA'):
            self.enc = enc = fec_dev.ra_encoder.make(frame_bits)
            self.dec = dec = fec_dev.ra_decoder.make(frame_bits)
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

def main(frame_bytes, num_frames, frame_error_count, ebn0, codec, t_errors, max_seconds, max_frames, interleaver):
    bytes_per_vector = frame_bytes * num_frames
    bits_per_vector = bytes_per_vector * BITS_PER_BYTE

    now = datetime.datetime.now()
    filename = f'gr-fec-testbench-{now}.csv'
    info = [['# '],
            ['# ----- GNU Radio FEC Testbench -----'],
            ['# '],
            ['# Date:                   ',f'{now}'],
            ['# Codec:                  ',f'{codec}'],
            ['# Interleaver (turbo/RA): ',f'{interleaver}'],
            ['# t (BCH/RS only):        ',f'{t_errors}'],
            ['# Frame bytes (bits):     ',f'{frame_bytes} ({frame_bytes*BITS_PER_BYTE})'],
            ['# Frames per message:     ',f'{num_frames}'],
            ['# Frame errors:           ',f'{frame_error_count}'],
            ['# Min Eb/N0:              ',f'{min(ebn0)}'],
            ['# Max Eb/N0:              ',f'{max(ebn0)}'],
            ['# Max simulation sec:     ',f'{max_seconds}'],
            ['# Max simulation frames:  ',f'{max_frames}'],
            ['# '],
            [],
            ['Eb/N0 (dB)','Es/N0 (dB)','Sigma','Frames\t','BE','BER','FE','FER','Time elapsed','Throughput (Mb/s)']
    ]

    for item in info:
        for inner in item:
            print(f'{inner:15}', end='')
        print()

    with open(filename, 'w') as csvfile:
        csvwriter = csv.writer(csvfile)
        csvwriter.writerows(info)

        for snr in ebn0:
            bit_errors = frame_errors = loops = 0
            start = timeit.default_timer()

            abandon = False
            while (frame_errors < frame_error_count) and not abandon:
                random_generator = np.random.default_rng()
                vector = list(random_generator.bytes(bytes_per_vector))

                fg = test_fg(vector, frame_bytes, snr, codec, t_errors, interleaver)
                fg.start()
                fg.wait()

                src_data = np.array(fg.src_b.data())
                dec_data = np.array(fg.dec_b.data())

                diff = src_data ^ dec_data
                bit_errors += np.count_nonzero(diff)

                src_frames = np.reshape(np.packbits(src_data), (num_frames, frame_bytes))
                dec_frames = np.reshape(np.packbits(dec_data), (num_frames, frame_bytes))

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
                formatted_time = time.strftime('%H:%M:%S', time.gmtime(elapsed_time))
                throughput = total_bits/elapsed_time / 1E6

                sigma = fg.sigma
                esn0 = fg.esn0
                
                print(f'{snr:<15}{esn0:<15.2f}{sigma:<15.4f}{total_frames:<15}\t{bit_errors:<15}{ber:<15.3E}{frame_errors:<15}{fer:<15.3E}{formatted_time:15}{throughput:<15.2f}', end='\r')

                abandon = max_seconds > 0 and elapsed_time > max_seconds or max_frames > 0 and total_frames > max_frames
                if abandon:
                    print('end of simulation step due to time or number of frames limit')
            
            print()
            csvwriter.writerow([snr, esn0, sigma, total_frames, bit_errors, ber, frame_errors, fer, formatted_time, throughput])

    return True

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='') # TODO add description

    parser.add_argument('-c', '--codec', choices=['turbo', 'BCH', 'RA'], default='turbo',
                        help='encoding and decoding scheme')
    parser.add_argument('-i', '--interleaver', choices=['NO', 'COL_ROW', 'ROW_COL', 'RANDOM', 'RAND_COL',
                                'GOLDEN', 'CCSDS', 'LTE', 'DVB_RCS1', 'DVB_RCS2'], default='NO',
                        help='if applicable for codec (RA, turbo), interleaver type ... \
                        for turbo, sets additional parameters based on choice (polynomials, trellis size)')
    parser.add_argument('-t', '--correctable_errors', type=int, default=0,
                        help='if applicable for codec (BCH, RS), number of errors that can be corrected')

    parser.add_argument('-b', '--frame_bytes', type=int, default=20,
                        help='number of bytes within one frame')
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
    frame_bytes = args.frame_bytes
    codec = args.codec
    t_errors = args.correctable_errors
    interleaver = args.interleaver
    max_seconds = args.max_seconds
    max_frames = args.max_frames
    frame_error_count = args.frame_error_count

    if args.num_frames % 4:
        print('Number of frames (-f, --num_frames) must be divisible by 4')
        sys.exit(1)
    if interleaver == 'CCSDS' and frame_bytes not in CCSDS_FRAME_SIZES:
        print(f'CCSDS supported frame sizes are {CCSDS_FRAME_SIZES}')
        sys.exit(1)

    if args.ebn0_list:
        ebn0 = args.ebn0_list
    else:
        ebn0 = np.round(np.arange(args.min_ebn0, args.max_ebn0 + args.snr_step, args.snr_step), int(np.log10(1/args.snr_step)))
    
    
    # TODO BCH
    sys.exit(not main(frame_bytes, num_frames, frame_error_count, ebn0, codec, t_errors, max_seconds, max_frames, interleaver))



