/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/fec_dev/bch_decoder.h>
#include <gnuradio/io_signature.h>
#include "bch_decoder_impl.h"

namespace gr {
namespace fec_dev {

fec::generic_decoder::sptr bch_decoder::make(int frame_size)
{
    return fec::generic_decoder::sptr(new bch_decoder_impl(frame_size));
}
    bch_decoder_impl::bch_decoder_impl(int frame_size)
        : generic_decoder("bch_decoder"),
        d_max_frame_size(frame_size)
    {
        set_frame_size(frame_size);

        //create decoder
}

bch_decoder_impl::~bch_decoder_impl()
{
}

int bch_decoder_impl::get_output_size() { return d_output_size; }
int bch_decoder_impl::get_input_size() { return d_input_size; }

bool bch_decoder_impl::set_frame_size(unsigned int frame_size)
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
    d_input_size = frame_size;
    d_output_size = frame_size;

    return ret;
}

double bch_decoder_impl::rate() { return d_frame_size / d_input_size; } // decoder rate

void bch_decoder_impl::generic_work(const void* inbuffer, void* outbuffer)
{
    const unsigned char* in = (const unsigned char*)inbuffer;
    unsigned char* out = (unsigned char*)outbuffer;

    // call decoder
    memcpy(out, in, d_frame_size * sizeof(char));
}


} /* namespace fec_dev */
} /* namespace gr */
