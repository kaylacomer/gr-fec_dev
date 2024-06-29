/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/fec_dev/rs_decoder.h>
#include <gnuradio/io_signature.h>
#include "rs_decoder_impl.h"

namespace gr {
namespace fec_dev {

fec::generic_decoder::sptr rs_decoder::make(int frame_size)
{
    return fec::generic_decoder::sptr(new rs_decoder_impl(frame_size));
}
    rs_decoder_impl::rs_decoder_impl(int frame_size)
        : generic_decoder("rs_decoder"),
        d_max_frame_size(frame_size)
    {
        set_frame_size(frame_size);

        //create decoder
}

rs_decoder_impl::~rs_decoder_impl()
{
}

int rs_decoder_impl::get_output_size() { return d_output_size; }
int rs_decoder_impl::get_input_size() { return d_input_size; }

bool rs_decoder_impl::set_frame_size(unsigned int frame_size)
{
    bool ret = true;
    if (frame_size > d_max_frame_size) {
        d_logger->info("tried to set frame to {:d}; max possible is {:d}",
                    frame_size,
                    d_max_frame_size);
        frame_size = d_max_frame_size;
        ret = false;
    }

    d_frame_size = frame_size;
    d_output_size = frame_size;
    d_input_size = frame_size;

    return ret;
}

double rs_decoder_impl::rate() { return d_output_size / d_input_size; } // decoder rate

void rs_decoder_impl::generic_work(const void* inbuffer, void* outbuffer)
{
    const unsigned char* in = (const unsigned char*)inbuffer;
    unsigned char* out = (unsigned char*)outbuffer;

    // call decoder
    memcpy(out, in, d_frame_size * sizeof(char));
}

} /* namespace fec_dev */
} /* namespace gr */
