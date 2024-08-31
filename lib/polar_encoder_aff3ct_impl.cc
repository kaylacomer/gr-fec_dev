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

fec::generic_encoder::sptr polar_encoder_aff3ct::make(int K,
                                                      int N,
                                                      float sigma,
                                                      Polar::frozen_bit_gen_t frozen_bit_gen,
                                                      Polar::noise_t noise_type)
                        {
    return fec::generic_encoder::sptr(std::make_shared<polar_encoder_aff3ct_impl>(K, N, sigma, frozen_bit_gen, noise_type));
}
    polar_encoder_aff3ct_impl::polar_encoder_aff3ct_impl(int K,
                                                      int N,
                                                      float sigma,
                                                      Polar::frozen_bit_gen_t frozen_bit_gen,
                                                      Polar::noise_t noise_type)
        : generic_encoder("polar_encoder_aff3ct"),
        d_K(K),
        d_N(N)
    {
        set_frame_size(K);

        std::vector<bool> frozen_bits(d_N);
        if (frozen_bit_gen == Polar::GA_ARIKAN) {
            d_frozen_bitgen = std::make_unique<aff3ct::tools::Frozenbits_generator_GA_Arikan>(d_K, d_N);
        }
        else {
            throw std::runtime_error("Only GA_ARIKAN frozen bit generator supported at this time");
        }

        if (noise_type == Polar::Sigma) {
            auto noise = std::make_unique<aff3ct::tools::Sigma<>>(sigma);
            d_frozen_bitgen->set_noise(*noise);
            d_frozen_bitgen->generate(frozen_bits);
        }
        else {
            throw std::runtime_error("Only Sigma noise supported at this time");
        }

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