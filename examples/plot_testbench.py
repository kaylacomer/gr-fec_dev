#!/usr/bin/env python3

import matplotlib.pyplot as plt
import glob
import pandas
import mplcursors

def parse(path, sep):
    for file in glob.glob(path):
        with open(file, 'r') as csvfile:
            meta = {}
            comment = True
        
            while comment:
                pos = csvfile.tell()
                line = csvfile.readline()
                if '#' not in line:
                    comment = False
                    csvfile.seek(pos)
                else:
                    info = line.split(',')
                    if (len(info) > 1):
                        meta[info[0].strip('# ')] = info[1].strip()
            
            meta = pandas.Series(meta, name='Metadata')
            data = pandas.read_csv(csvfile, sep=sep)

            line, = plt.semilogy('Eb/N0 (dB)', 'BER', data=data, ls='-', marker='.', lw=1)
            color = line.get_color()
            plt.semilogy('Eb/N0 (dB)', 'FER', data=data, ls='--', marker='.', lw=1, color=color)

parse('gr-fec-testbench*', ',')

plt.grid(which='both')
plt.xlabel('Signal-to-noise ratio, Eb/N0 (dB)')
plt.ylabel('Error rate (BER solid, FER dashed)')

mplcursors.cursor(hover=True)

plt.show()