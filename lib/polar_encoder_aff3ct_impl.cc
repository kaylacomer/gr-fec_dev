/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/io_signature.h>
#include "polar_encoder_aff3ct_impl.h"

namespace gr {
namespace fec_dev {

fec::generic_encoder::sptr polar_encoder_aff3ct::make(int K)
{
    return fec::generic_encoder::sptr(new polar_encoder_aff3ct_impl(K));
}
    polar_encoder_aff3ct_impl::polar_encoder_aff3ct_impl(int K)
        : generic_encoder("polar_encoder_aff3ct"),
        d_K(K),
        d_N(256)
    {
        set_frame_size(K);

        d_frozen_bitgen = std::make_unique<aff3ct::tools::Frozenbits_generator_GA_Arikan>(d_K, d_N);
        std::vector<bool> frozen_bits(d_N);
        float sigma = 0.6692;
        auto noise = std::make_unique<aff3ct::tools::Sigma<>>(sigma);
        d_frozen_bitgen->set_noise(*noise);
        d_frozen_bitgen->generate(frozen_bits);

        d_encoder = std::make_unique<aff3ct::module::Encoder_polar_sys<B_8>>(d_K, d_N, frozen_bits);
}

polar_encoder_aff3ct_impl::~polar_encoder_aff3ct_impl()
{
}

int polar_encoder_aff3ct_impl::get_output_size() { return d_N; }
int polar_encoder_aff3ct_impl::get_input_size() { return d_K; }

bool polar_encoder_aff3ct_impl::set_frame_size(unsigned int K)
{
    return true;
}

double polar_encoder_aff3ct_impl::rate() { return static_cast<float>(d_K) / d_N; } // encoder rate

void polar_encoder_aff3ct_impl::generic_work(const void* inbuffer, void* outbuffer)
{
    const B_8* in = (const B_8*)inbuffer;
    B_8* out = (B_8*)outbuffer;

    d_encoder->encode(in, out);
}

} /* namespace fec_dev */
} /* namespace gr */