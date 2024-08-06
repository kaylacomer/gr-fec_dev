/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/io_signature.h>
#include "tpc_decoder_aff3ct_impl.h"

namespace gr {
namespace fec_dev {

fec::generic_decoder::sptr tpc_decoder_aff3ct::make(int K)
{
    return fec::generic_decoder::sptr(new tpc_decoder_aff3ct_impl(K));
}
    tpc_decoder_aff3ct_impl::tpc_decoder_aff3ct_impl(int K)
        : generic_decoder("tpc_decoder_aff3ct"),
        d_K(K)
    {
        set_frame_size(K);

        //d_decoder = std::make_unique<aff3ct::module::Encoder_RA<B_8>>(d_K, d_N, *d_interleaver);
}

tpc_decoder_aff3ct_impl::~tpc_decoder_aff3ct_impl()
{
}

int tpc_decoder_aff3ct_impl::get_output_size() { return d_N; }
int tpc_decoder_aff3ct_impl::get_input_size() { return d_K; }

bool tpc_decoder_aff3ct_impl::set_frame_size(unsigned int K)
{
    return true;
}

double tpc_decoder_aff3ct_impl::rate() { return static_cast<float>(d_K) / d_N; } // decoder rate

void tpc_decoder_aff3ct_impl::generic_work(const void* inbuffer, void* outbuffer)
{
    const B_8* in = (const B_8*)inbuffer;
    B_8* out = (B_8*)outbuffer;

    //d_decoder->decode(in, out);
}

} /* namespace fec_dev */
} /* namespace gr */