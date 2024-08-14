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

fec::generic_decoder::sptr bch_decoder::make(int frame_bits, uint8_t t)
{
    return fec::generic_decoder::sptr(new bch_decoder_impl(frame_bits, t));
}
    bch_decoder_impl::bch_decoder_impl(int frame_bits, uint8_t t)
        : generic_decoder("bch_decoder"),
        d_frame_size(frame_bits),
        d_t(t)
    {
        set_frame_size(frame_bits);

        uint8_t m = std::ceil(std::log2(frame_bits+1));
        d_N = (1 << m) - 1;
        d_poly_gen = std::make_unique<aff3ct::tools::BCH_polynomial_generator<B_8>>(d_N, d_t);
        d_K = d_N - d_poly_gen->get_n_rdncy();
        d_zeros = d_K - d_frame_size;
        d_codeword_size = d_N - d_zeros;

        if (d_K < 3) {
            throw std::runtime_error("K < 3 - frame_bits and t incompatible. Reduce t or increase frame_bits");
        }

        d_quant = std::make_unique<aff3ct::module::Quantizer_pow2_fast<float, Q_8>>(d_codeword_size, 2);
        d_quant_input = std::vector<Q_8>(d_N);
        d_tmp_input = std::vector<float>(d_codeword_size);
        d_tmp_output = std::vector<B_8>(d_K);

        d_decoder = std::make_unique<aff3ct::module::Decoder_BCH_std<B_8, Q_8>>(d_K, d_N, *d_poly_gen);
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

    std::cout << "status: " << status << " ---- SUCCESS 0 , FAILURE 1" << std::endl;
    std::memcpy(out, &d_tmp_output[d_zeros], d_frame_size * sizeof(B_8));
    // if (status == spu::runtime::status_t::SUCCESS) {
    //     std::memcpy(out, tmp_output.data(), d_frame_size * sizeof(B_8));
    // }
    // else {
    //     d_logger->info("Decoding failed");
    //     std::fill(out, out + d_frame_size, 0);
    // }
}


} /* namespace fec_dev */
} /* namespace gr */
