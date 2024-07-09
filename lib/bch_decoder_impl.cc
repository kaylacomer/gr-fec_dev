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

        d_quant = std::make_unique<aff3ct::module::Quantizer_pow2_fast<float, Q_8>>(d_N, 2);
        d_quant_input = std::vector<Q_8>(d_N);
        d_tmp_input = std::vector<float>(d_N);

        d_poly_gen = std::make_unique<aff3ct::tools::BCH_polynomial_generator<B_8>>(d_N, t);

        d_decoder = std::make_unique<aff3ct::module::Decoder_BCH_std<B_8, Q_8>>(d_K, d_N, *d_poly_gen);
}

bch_decoder_impl::~bch_decoder_impl()
{
}

int bch_decoder_impl::get_output_size() { return d_frame_size; }
int bch_decoder_impl::get_input_size() { return d_N; }

bool bch_decoder_impl::set_frame_size(unsigned int frame_bits)
{
    bool ret = true;

    uint8_t m = std::ceil(std::log2(frame_bits+1));
    d_N = std::pow(2,m)-1;
    d_K = d_N - m*d_t;

    return ret;
}

double bch_decoder_impl::rate() { return d_frame_size / d_N; } // decoder rate

void bch_decoder_impl::generic_work(const void* inbuffer, void* outbuffer)
{
    const float* in = (const float*)inbuffer;
    B_8* out = (B_8*)outbuffer;

    volk_32f_s32f_multiply_32f(d_tmp_input.data(), in, -1.0f, d_N);
    d_quant->process(d_tmp_input.data(), d_quant_input.data(), -1);
    
    std::vector<B_8> temp_output(d_K);
    d_decoder->decode_siho(d_quant_input.data(), temp_output.data(), -1);

    std::memcpy(out, temp_output.data(), d_frame_size * sizeof(B_8));
}


} /* namespace fec_dev */
} /* namespace gr */
