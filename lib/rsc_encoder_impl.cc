/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/io_signature.h>
#include "rsc_encoder_impl.h"

namespace gr {
namespace fec_dev {

fec::generic_encoder::sptr rsc_encoder::make(int K)
{
    return fec::generic_encoder::sptr(new rsc_encoder_impl(K));
}
    rsc_encoder_impl::rsc_encoder_impl(int K)
        : generic_encoder("rsc_encoder"),
        d_K(K)
    {
        set_frame_size(K);

        //d_encoder = std::make_unique<aff3ct::module::Encoder_RA<B_8>>(d_K, d_N, *d_interleaver);
}

rsc_encoder_impl::~rsc_encoder_impl()
{
}

int rsc_encoder_impl::get_output_size() { return d_N; }
int rsc_encoder_impl::get_input_size() { return d_K; }

bool rsc_encoder_impl::set_frame_size(unsigned int K)
{
    return true;
}

double rsc_encoder_impl::rate() { return static_cast<float>(d_K) / d_N; } // encoder rate

void rsc_encoder_impl::generic_work(const void* inbuffer, void* outbuffer)
{
    const B_8* in = (const B_8*)inbuffer;
    B_8* out = (B_8*)outbuffer;

    //d_encoder->encode(in, out);
}

} /* namespace fec_dev */
} /* namespace gr */