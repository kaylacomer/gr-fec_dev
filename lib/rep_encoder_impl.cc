/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/io_signature.h>
#include "rep_encoder_impl.h"

namespace gr {
namespace fec_dev {

fec::generic_encoder::sptr rep_encoder::make(int K, int rep, bool buffered)
{
    return fec::generic_encoder::sptr(std::make_shared<rep_encoder_impl>(K, rep, buffered));
}
    rep_encoder_impl::rep_encoder_impl(int K, int rep, bool buffered)
        : generic_encoder("rep_encoder"),
        d_K(K),
        d_N(rep * K)
    {
        set_frame_size(K);

        d_encoder = std::make_unique<aff3ct::module::Encoder_repetition_sys<B_8>>(d_K, d_N);
}

rep_encoder_impl::~rep_encoder_impl()
{
}

int rep_encoder_impl::get_output_size() { return d_N; }
int rep_encoder_impl::get_input_size() { return d_K; }

bool rep_encoder_impl::set_frame_size(unsigned int K)
{
    return true;
}

double rep_encoder_impl::rate() { return static_cast<float>(d_K) / d_N; } // encoder rate

void rep_encoder_impl::generic_work(const void* inbuffer, void* outbuffer)
{
    const B_8* in = (const B_8*)inbuffer;
    B_8* out = (B_8*)outbuffer;

    d_encoder->encode(in, out);
}

} /* namespace fec_dev */
} /* namespace gr */
