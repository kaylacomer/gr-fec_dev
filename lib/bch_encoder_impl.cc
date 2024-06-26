/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "bch_encoder_impl.h"
#include <gnuradio/fec_dev/bch_encoder.h>
#include <gnuradio/io_signature.h>

namespace gr {
namespace fec_dev {

fec::generic_encoder::sptr bch_encoder::make(int frame_size)
{
    return fec::generic_encoder::sptr(new bch_encoder_impl(frame_size));
}
    /*
     * The private constructor
     */
    bch_encoder_impl::bch_encoder_impl(int frame_size)
        : generic_encoder("bch_encoder"),
        d_max_frame_size(frame_size)
    {
        set_frame_size(frame_size);

        //create encoder
}

/*
* Our virtual destructor.
*/
bch_encoder_impl::~bch_encoder_impl()
{
}

int bch_encoder_impl::get_output_size() { return d_output_size; }
int bch_encoder_impl::get_input_size() { return d_frame_size; }

bool bch_encoder_impl::set_frame_size(unsigned int frame_size)
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

    return ret;
}

double bch_encoder_impl::rate() { return d_frame_size / d_output_size; }

void bch_encoder_impl::generic_work(const void* inbuffer, void* outbuffer)
{
    const unsigned char* in = (const unsigned char*)inbuffer;
    unsigned char* out = (unsigned char*)outbuffer;

    // call encoder
    memcpy(out, in, d_frame_size * sizeof(char));
}

} /* namespace fec_dev */
} /* namespace gr */
