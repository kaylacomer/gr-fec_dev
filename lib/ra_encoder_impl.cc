/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/fec_dev/ra_encoder.h>
#include <gnuradio/io_signature.h>
#include "ra_encoder_impl.h"

namespace gr {
namespace fec_dev {

fec::generic_encoder::sptr ra_encoder::make(int frame_size)
{
    return fec::generic_encoder::sptr(new ra_encoder_impl(frame_size));
}
    ra_encoder_impl::ra_encoder_impl(int frame_size)
        : generic_encoder("ra_encoder"),
        d_max_frame_size(frame_size)
    {
        set_frame_size(frame_size);

        //create encoder
}

ra_encoder_impl::~ra_encoder_impl()
{
}

int ra_encoder_impl::get_output_size() { return d_output_size; }
int ra_encoder_impl::get_input_size() { return d_input_size; }

bool ra_encoder_impl::set_frame_size(unsigned int frame_size)
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

double ra_encoder_impl::rate() { return d_input_size / d_output_size; } // encoder rate

void ra_encoder_impl::generic_work(const void* inbuffer, void* outbuffer)
{
    const unsigned char* in = (const unsigned char*)inbuffer;
    unsigned char* out = (unsigned char*)outbuffer;

    // call encoder
    memcpy(out, in, d_frame_size * sizeof(char));
}

} /* namespace fec_dev */
} /* namespace gr */
