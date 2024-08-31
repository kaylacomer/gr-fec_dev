/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/fec_dev/bch_decoder.h>
#include <gnuradio/io_signature.h>
#include "bch_decoder_impl.h"
#include <volk/volk.h>

namespace gr {
namespace fec_dev {

fec::generic_decoder::sptr bch_decoder::make(int frame_bits, uint8_t t, uint8_t quant_fixed_point_pos, uint8_t 
                    quant_saturation_pos, Quantizer::quantizer_impl_t quant_impl, Decoder::decoder_impl_t dec_impl,
                    SIMD::simd_strat_t simd_strat)
{
    return fec::generic_decoder::sptr(std::make_shared<bch_decoder_impl>(frame_bits, t, quant_fixed_point_pos, quant_saturation_pos, quant_impl, dec_impl, simd_strat));
}
    bch_decoder_impl::bch_decoder_impl(int frame_bits, uint8_t t, uint8_t quant_fixed_point_pos, uint8_t 
                    quant_saturation_pos, Quantizer::quantizer_impl_t quant_impl, Decoder::decoder_impl_t dec_impl,
                    SIMD::simd_strat_t simd_strat)
        : generic_decoder("bch_decoder"),
        d_frame_size(frame_bits)
    {
        set_frame_size(frame_bits);

        uint8_t m = std::ceil(std::log2(frame_bits+1));
        d_N = (1 << m) - 1;
        d_poly_gen = std::make_unique<aff3ct::tools::BCH_polynomial_generator<B_32>>(d_N, t);
        d_K = d_N - d_poly_gen->get_n_rdncy();
        d_zeros = d_K - d_frame_size;
        d_codeword_size = d_N - d_zeros;

        if (d_zeros < 0) {
            m += 1;
            d_N = (1 << m) - 1;

            d_poly_gen = std::make_unique<aff3ct::tools::BCH_polynomial_generator<B_32>>(d_N, t);
            d_K = d_N - d_poly_gen->get_n_rdncy();

            d_zeros = d_K - d_frame_size;
            d_codeword_size = d_N - d_zeros;
        }

        if (d_K < 3) {
            throw std::runtime_error("K < 3 - frame_bits and t incompatible. Reduce t or increase frame_bits");
        }

        if (quant_impl == Quantizer::STD) {
            d_quant = std::make_unique<aff3ct::module::Quantizer_pow2<float, Q_32>>(d_codeword_size, quant_fixed_point_pos, quant_saturation_pos);
        }
        else if (quant_impl == Quantizer::FAST) {
            d_quant = std::make_unique<aff3ct::module::Quantizer_pow2_fast<float, Q_32>>(d_codeword_size, quant_fixed_point_pos, quant_saturation_pos);
        }
        else {
            d_quant = std::make_unique<aff3ct::module::Quantizer_NO<float, Q_32>>(d_codeword_size);
        }
        d_quant_input = std::vector<Q_32>(d_N);
        d_tmp_input = std::vector<float>(d_codeword_size);
        d_tmp_output = std::vector<B_32>(d_K);

        if (dec_impl == Decoder::STD) {
            d_decoder = std::make_unique<aff3ct::module::Decoder_BCH_std<B_32, Q_32>>(d_K, d_N, *d_poly_gen);
        }
        else if (dec_impl == Decoder::FAST) {
            d_decoder = std::make_unique<aff3ct::module::Decoder_BCH_fast<B_32, Q_32>>(d_K, d_N, *d_poly_gen);
        }
        else if (dec_impl == Decoder::GENIUS) {
            if (simd_strat == SIMD::SEQ) {
                auto encoder = std::make_unique<aff3ct::module::Encoder_BCH<B_32>>(d_K, d_N, *d_poly_gen);
                d_decoder = std::make_unique<aff3ct::module::Decoder_BCH_genius<B_32, Q_32>>(d_K, d_N, t, *encoder);
            }
            else if (simd_strat == SIMD::INTER) {
                auto encoder = std::make_unique<aff3ct::module::Encoder_BCH_inter<B_32>>(d_K, d_N, *d_poly_gen);
                d_decoder = std::make_unique<aff3ct::module::Decoder_BCH_genius<B_32, Q_32>>(d_K, d_N, t, *encoder);
            }
            else {
                throw std::runtime_error("SIMD for BCH must be sequential or inter. Set simd_strat to SEQ or INTER");
            }
        }
        else {
            throw std::runtime_error("BCH decoder has STD, FAST, and GENIUS implementations");
        }
}

bch_decoder_impl::~bch_decoder_impl()
{
}

int bch_decoder_impl::get_output_size() { return d_frame_size; }
int bch_decoder_impl::get_input_size() { return d_codeword_size; }

bool bch_decoder_impl::set_frame_size(unsigned int frame_bits)
{
    bool ret = true;

    return ret;
}

double bch_decoder_impl::rate() { return static_cast<float>(d_frame_size) / d_codeword_size; } // decoder rate

void bch_decoder_impl::generic_work(const void* inbuffer, void* outbuffer)
{
    const float* in = (const float*)inbuffer;
    B_8* out = (B_8*)outbuffer;

    volk_32f_s32f_multiply_32f(d_tmp_input.data(), in, -1.0f, d_codeword_size);
    d_quant->process(d_tmp_input.data(), &d_quant_input[d_zeros], -1);
    
    int status = d_decoder->decode_siho(d_quant_input.data(), d_tmp_output.data(), -1);

    // if (status == spu::runtime::status_t::SUCCESS) {
        // std::memcpy(out, &d_tmp_output[d_zeros], d_frame_size);
    // }
    // else {
    //     d_logger->info("Decoding failed");
    //     std::fill(out, out + d_frame_size, 0);
    // }
    for (int i = 0; i < d_tmp_output.size(); i++) {
        if ((d_tmp_output[i] != 0) && (d_tmp_output[i] != 1)) {
            __builtin_trap();
        }
    }
    for (int i = 0; i < d_frame_size; i++) {
        out[i] = static_cast<B_8>(d_tmp_output[d_zeros + i]);
        if (static_cast<B_32>(out[i]) != d_tmp_output[d_zeros + i]) {
            std::cout << "out: " << static_cast<B_32>(out[i]) << std::endl;
            std::cout << "tmp out: " << d_tmp_output[d_zeros + i] << std::endl;
        }
    }
}


} /* namespace fec_dev */
} /* namespace gr */
