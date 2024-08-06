/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/fec_dev/ra_encoder.h>
#include <gnuradio/io_signature.h>
#include "ra_encoder_impl.h"
#include "Tools/Interleaver/NO/Interleaver_core_NO.hpp"

namespace gr {
namespace fec_dev {

fec::generic_encoder::sptr ra_encoder::make(int K, int rep)
{
    return fec::generic_encoder::sptr(new ra_encoder_impl(K, rep));
}
    ra_encoder_impl::ra_encoder_impl(int K, int rep)
        : generic_encoder("ra_encoder"),
        d_K(K),
        d_N(rep * K)
    {
        set_frame_size(K);

        d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_NO<>>(d_N);
        d_interleaver = std::make_unique<aff3ct::module::Interleaver<B_8>>(*d_interleaver_core);
        d_encoder = std::make_unique<aff3ct::module::Encoder_RA<B_8>>(d_K, d_N, *d_interleaver);
}

ra_encoder_impl::~ra_encoder_impl()
{
}

int ra_encoder_impl::get_output_size() { return d_N; }
int ra_encoder_impl::get_input_size() { return d_K; }

bool ra_encoder_impl::set_frame_size(unsigned int K)
{
    return true;
}

double ra_encoder_impl::rate() { return static_cast<float>(d_K) / d_N; } // encoder rate

void ra_encoder_impl::generic_work(const void* inbuffer, void* outbuffer)
{
    const B_8* in = (const B_8*)inbuffer;
    B_8* out = (B_8*)outbuffer;

    d_encoder->encode(in, out);
}

} /* namespace fec_dev */
} /* namespace gr */
