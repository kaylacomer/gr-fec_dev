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

fec::generic_decoder::sptr polar_decoder_aff3ct::make(int K,
                                                      int N,
                                                      float sigma,
                                                      Polar::frozen_bit_gen_t frozen_bit_gen,
                                                      Polar::noise_t noise_type,
                                                      Polar::decoder_t decoder_type,
                                                      Decoder::decoder_impl_t dec_impl,
                                                      uint8_t quant_fixed_point_pos,
                                                      uint8_t quant_saturation_pos,
                                                      Quantizer::quantizer_impl_t quant_impl)
{
    return fec::generic_decoder::sptr(std::make_shared<polar_decoder_aff3ct_impl>(K, N, sigma, frozen_bit_gen, 
            noise_type, decoder_type, dec_impl, quant_fixed_point_pos, quant_saturation_pos, quant_impl));
}
    polar_decoder_aff3ct_impl::polar_decoder_aff3ct_impl(int K,
                                                         int N,
                                                         float sigma,
                                                         Polar::frozen_bit_gen_t frozen_bit_gen,
                                                         Polar::noise_t noise_type,
                                                         Polar::decoder_t decoder_type,
                                                         Decoder::decoder_impl_t dec_impl,
                                                         uint8_t quant_fixed_point_pos,
                                                         uint8_t quant_saturation_pos,
                                                         Quantizer::quantizer_impl_t quant_impl)
        : generic_decoder("polar_decoder_aff3ct"),
        d_K(K),
        d_N(256)
    {
        set_frame_size(K);

        if (frozen_bit_gen == Polar::GA_ARIKAN) {
            d_frozen_bitgen = std::make_unique<aff3ct::tools::Frozenbits_generator_GA_Arikan>(d_K, d_N);
        }
        else {
            throw std::runtime_error("Only GA_ARIKAN frozen bit generator supported at this time");
        }

        if (noise_type == Polar::Sigma) {
            auto noise = std::make_unique<aff3ct::tools::Sigma<>>(sigma);
            d_frozen_bitgen->set_noise(*noise);
        }
        else {
            throw std::runtime_error("Only Sigma noise supported at this time");
        }

        std::vector<bool> frozen_bits(d_N);
        d_frozen_bitgen->generate(frozen_bits);

        d_quant = std::make_unique<aff3ct::module::Quantizer_pow2_fast<float, Q_8>>(d_N, 2);
        d_quant_input = std::vector<Q_8>(d_N);
        d_tmp_input = std::vector<float>(d_N);

        if (quant_impl == Quantizer::STD) {
            d_quant = std::make_unique<aff3ct::module::Quantizer_pow2<float, Q_8>>(d_N, quant_fixed_point_pos, quant_saturation_pos);
        }
        else if (quant_impl == Quantizer::FAST) {
            d_quant = std::make_unique<aff3ct::module::Quantizer_pow2_fast<float, Q_8>>(d_N, quant_fixed_point_pos, quant_saturation_pos);
        }
        else {
            d_quant = std::make_unique<aff3ct::module::Quantizer_NO<float, Q_8>>(d_N);
        }
        
        // using API_polar = aff3ct::tools::API_polar_static_intra_8bit<B_8, Q_8>;
        // d_decoder = std::make_unique<aff3ct::module::Decoder_polar_SC_fast_sys<B_8, Q_8, API_polar>>(d_K, d_N, frozen_bits);
        if (dec_impl == Decoder::NAIVE) {
            d_decoder = std::make_unique<aff3ct::module::Decoder_polar_SC_naive_sys<B_8, Q_8>>(d_K, d_N, frozen_bits);
        }
        else {
            throw std::runtime_error("Only NAIVE noise supported at this time");
        }
}

polar_decoder_aff3ct_impl::~polar_decoder_aff3ct_impl()
{
}

int polar_decoder_aff3ct_impl::get_output_size() { return d_K; }
int polar_decoder_aff3ct_impl::get_input_size() { return d_N; }

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
    
    int status = d_decoder->decode_siho(d_quant_input.data(), out, -1);
    if (status == 1) {
        std::cout << "Decoding FAILURE" << std::endl;
    }
}

} /* namespace fec_dev */
} /* namespace gr */