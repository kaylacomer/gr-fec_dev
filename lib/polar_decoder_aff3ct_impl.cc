/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/io_signature.h>
#include "polar_decoder_aff3ct_impl.h"
#include <volk/volk.h>

namespace gr {
namespace fec_dev {

fec::generic_decoder::sptr polar_decoder_aff3ct::make(int K)
{
    return fec::generic_decoder::sptr(std::make_shared<polar_decoder_aff3ct_impl>(K));
}
    polar_decoder_aff3ct_impl::polar_decoder_aff3ct_impl(int K)
        : generic_decoder("polar_decoder_aff3ct"),
        d_K(K),
        d_N(256)
    {
        set_frame_size(K);

        d_quant = std::make_unique<aff3ct::module::Quantizer_pow2_fast<float, Q_8>>(d_N, 2);
        d_quant_input = std::vector<Q_8>(d_N);
        d_tmp_input = std::vector<float>(d_N);

        d_frozen_bitgen = std::make_unique<aff3ct::tools::Frozenbits_generator_GA_Arikan>(d_K, d_N);
        std::vector<bool> frozen_bits(d_N);
        float sigma = 0.6692;
        auto noise = std::make_unique<aff3ct::tools::Sigma<>>(sigma);
        d_frozen_bitgen->set_noise(*noise);
        d_frozen_bitgen->generate(frozen_bits);

        // using API_polar = aff3ct::tools::API_polar_static_intra_8bit<B_8, Q_8>;
        // d_decoder = std::make_unique<aff3ct::module::Decoder_polar_SC_fast_sys<B_8, Q_8, API_polar>>(d_K, d_N, frozen_bits);
        d_decoder = std::make_unique<aff3ct::module::Decoder_polar_SC_naive_sys<B_8, Q_8>>(d_K, d_N, frozen_bits);
}

polar_decoder_aff3ct_impl::~polar_decoder_aff3ct_impl()
{
}

int polar_decoder_aff3ct_impl::get_output_size() { return d_N; }
int polar_decoder_aff3ct_impl::get_input_size() { return d_K; }

bool polar_decoder_aff3ct_impl::set_frame_size(unsigned int K)
{
    return true;
}

double polar_decoder_aff3ct_impl::rate() { return static_cast<float>(d_K) / d_N; } // decoder rate

void polar_decoder_aff3ct_impl::generic_work(const void* inbuffer, void* outbuffer)
{
    const float* in = (const float*)inbuffer;
    B_8* out = (B_8*)outbuffer;

    volk_32f_s32f_multiply_32f(d_tmp_input.data(), in, -1.0f, d_N);
    d_quant->process(d_tmp_input.data(), d_quant_input.data(), -1);
    
    std::vector<B_8> temp_output(d_K);
    int status = d_decoder->decode_siho(d_quant_input.data(), temp_output.data(), -1);
    if (status == 1) {
        std::cout << "Decoding FAILURE" << std::endl;
    }

    std::memcpy(out, temp_output.data(), d_K * sizeof(B_8));
}

} /* namespace fec_dev */
} /* namespace gr */