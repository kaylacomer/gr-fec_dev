#!/usr/bin/env python3

import matplotlib.pyplot as plt
import glob
import pandas
import mplcursors

fig, ax = plt.figure()
annot = ax.annotate("", xy=(0,0), xytext=(-10,10),textcoords="offset points",
                    bbox=dict(boxstyle="round", fc="w"))
annot.set_visible(False)

for file in glob.glob('gr-fec-testbench*'):
    lines = []
    with open(file, 'r') as csvfile:
        meta = {}
        comment = True
        while comment:
            line = csvfile.readline()
            if '#' not in line:
                comment = False
            else:
                info = line.split(',')
                if (len(info) > 1):
                    meta[info[0].strip('# ')] = info[1].strip()
        
        data = pandas.read_csv(csvfile)

        line, = plt.semilogy('Eb/N0 (dB)', 'BER', data=data, ls='-', marker='.', lw=1)
        lines.append(line)
        color = line.get_color()
        line, = plt.semilogy('Eb/N0 (dB)', 'FER', data=data, ls='--', marker='.', lw=1, color=color)
        lines.append(line)

plt.grid(which='both')
plt.xlabel('Signal-to-noise ratio, Eb/N0 (dB)')
plt.ylabel('Error rate (BER solid, FER dashed)')

def update_annot(ind):
    x,y = line.get_data()
    annot.xy = (x[ind["ind"][0]], y[ind["ind"][0]])
    text = f'{x[ind["ind"][0]]} dB, {y[ind["ind"][0]]:.2E}'
    # text = "{}, {}".format(" ".join(list(map(str,ind["ind"]))), 
    #                        " ".join([names[n] for n in ind["ind"]]))
    annot.set_text(text)
    annot.get_bbox_patch().set_alpha(0.4)


def hover(event):
    vis = annot.get_visible()
    if event.inaxes == ax:
        cont, ind = line.contains(event)
        if cont:
            print(event)
            update_annot(ind)
            annot.set_visible(True)
            fig.canvas.draw_idle()
        else:
            if vis:
                annot.set_visible(False)
                fig.canvas.draw_idle()

fig.canvas.mpl_connect("motion_notify_event", hover)
    
plt.show()