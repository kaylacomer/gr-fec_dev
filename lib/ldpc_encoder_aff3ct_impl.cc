/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/io_signature.h>
#include "ldpc_encoder_aff3ct_impl.h"

namespace gr {
namespace fec_dev {

fec::generic_encoder::sptr ldpc_encoder_aff3ct::make(int K)
{
    return fec::generic_encoder::sptr(new ldpc_encoder_aff3ct_impl(K));
}
    ldpc_encoder_aff3ct_impl::ldpc_encoder_aff3ct_impl(int K)
        : generic_encoder("ldpc_encoder_aff3ct"),
        d_K(K)
    {
        set_frame_size(K);

        //d_encoder = std::make_unique<aff3ct::module::Encoder_RA<B_8>>(d_K, d_N, *d_interleaver);
}

ldpc_encoder_aff3ct_impl::~ldpc_encoder_aff3ct_impl()
{
}

int ldpc_encoder_aff3ct_impl::get_output_size() { return d_N; }
int ldpc_encoder_aff3ct_impl::get_input_size() { return d_K; }

bool ldpc_encoder_aff3ct_impl::set_frame_size(unsigned int K)
{
    return true;
}

double ldpc_encoder_aff3ct_impl::rate() { return static_cast<float>(d_K) / d_N; } // encoder rate

void ldpc_encoder_aff3ct_impl::generic_work(const void* inbuffer, void* outbuffer)
{
    const B_8* in = (const B_8*)inbuffer;
    B_8* out = (B_8*)outbuffer;

    //d_encoder->encode(in, out);
}

} /* namespace fec_dev */
} /* namespace gr */