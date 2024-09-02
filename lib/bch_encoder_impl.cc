/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "bch_encoder_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace fec_dev {

fec::generic_encoder::sptr bch_encoder::make(int frame_bits, uint8_t t, SIMD::simd_strat_t simd_strat)
{
    return fec::generic_encoder::sptr(std::make_shared<bch_encoder_impl>(frame_bits, t, simd_strat));
}
    /*
     * The private constructor
     */
    bch_encoder_impl::bch_encoder_impl(int frame_bits, uint8_t t, SIMD::simd_strat_t simd_strat)
        : generic_encoder("bch_encoder"),
        d_frame_bits(frame_bits)
    {
        set_frame_size(frame_bits);

        uint8_t m = std::ceil(std::log2(frame_bits+1));
        d_N = (1 << m) - 1;

        d_poly_gen = std::make_unique<aff3ct::tools::BCH_polynomial_generator<B_32>>(d_N, t);
        d_K = d_N - d_poly_gen->get_n_rdncy();

        d_zeros = d_K - d_frame_bits;
        d_codeword_size = d_N - d_zeros;

        if (d_zeros < 0) {
            m += 1;
            d_N = (1 << m) - 1;

            d_poly_gen = std::make_unique<aff3ct::tools::BCH_polynomial_generator<B_32>>(d_N, t);
            d_K = d_N - d_poly_gen->get_n_rdncy();

            d_zeros = d_K - d_frame_bits;
            d_codeword_size = d_N - d_zeros;
        }
        
        if (d_zeros) {
            d_logger->info("Padding {:d}-bit input (K_info) with {:d} zeros. K: {:d}, N: {:d}, N_cw: {:d}, t: {:d}", frame_bits, d_zeros, d_K, d_N, d_codeword_size, t);
        }
        
        if (d_K < 3) {
            throw std::runtime_error("K < 3 - frame_bits and t incompatible. Reduce t or increase frame_bits");
        }

        d_tmp_input = std::vector<B_32>(d_K);
        d_tmp_output = std::vector<B_32>(d_N);

        if (simd_strat == SIMD::SEQ) {
            d_encoder = std::make_unique<aff3ct::module::Encoder_BCH<B_32>>(d_K, d_N, *d_poly_gen);
        }
        else if (simd_strat == SIMD::INTER) {
            d_encoder = std::make_unique<aff3ct::module::Encoder_BCH_inter<B_32>>(d_K, d_N, *d_poly_gen);
        }
        else {
            throw std::runtime_error("SIMD for BCH must be sequential or inter. Set simd_strat to SEQ or INTER");
        }
}

/*
* Our virtual destructor.
*/
bch_encoder_impl::~bch_encoder_impl()
{
}

int bch_encoder_impl::get_output_size() { return d_codeword_size; }
int bch_encoder_impl::get_input_size() { return d_frame_bits; }

bool bch_encoder_impl::set_frame_size(unsigned int frame_bits)
{
    bool ret = true;

    return ret;
}

double bch_encoder_impl::rate() { return static_cast<float>(d_codeword_size) / d_frame_bits; } // block output:input rate, inverse of codec rate

void bch_encoder_impl::generic_work(const void* inbuffer, void* outbuffer)
{
    const B_8* in = (const B_8*)inbuffer;
    B_8* out = (B_8*)outbuffer;
    
    /*
    * The AFF3CT library uses the input data type as part of the encoding process. When left as B_8 / int8_t, the encoder
    * is highly prohibitive and has a maximum m of 8 -> maximum N = 2^8-1 = 255. For better compatibility with expected use,
    * the input data are cast to B_32 / int32_t before encoding. The output data are cast to B_8 for the block's use.
    * Since the input and output data are unpacked and consist only of 0s and 1s, the type cast works.
    */
    for (int i = 0; i < d_frame_bits; ++i) {
        d_tmp_input[d_zeros + i] = static_cast<B_32>(in[i]);
        if (static_cast<B_32>(in[i]) != d_tmp_input[d_zeros + i]) {
            std::cout << "in: " << static_cast<B_32>(in[i]) << std::endl;
            std::cout << "tmp in: " << d_tmp_input[d_zeros + i] << std::endl;
        }
    }

    d_encoder->encode(d_tmp_input.data(), d_tmp_output.data());

    for (int i = 0; i < d_codeword_size; i++) {
        out[i] = static_cast<B_8>(d_tmp_output[d_zeros + i]);
        if (static_cast<B_32>(out[i]) != d_tmp_output[d_zeros + i]) {
            std::cout << "out: " << static_cast<B_32>(out[i]) << std::endl;
            std::cout << "tmp out: " << d_tmp_output[d_zeros + i] << std::endl;
        }
    }
    // std::memcpy(out, &d_tmp_output[d_zeros], d_codeword_size);
}

} /* namespace fec_dev */
} /* namespace gr */
