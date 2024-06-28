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

fec::generic_decoder::sptr bch_decoder::make(int codeword, uint8_t t)
{
    return fec::generic_decoder::sptr(new bch_decoder_impl(codeword, t));
}
    bch_decoder_impl::bch_decoder_impl(int codeword, uint8_t t)
        : generic_decoder("bch_decoder"),
        d_max_frame_size(codeword),
        d_t(t)
    {
        set_frame_size(codeword);

        d_quant = std::make_unique<aff3ct::module::Quantizer_pow2_fast<float, Q_8>>(d_input_size, 2);
        d_quant_input = std::vector<Q_8>(d_input_size);
        d_tmp_input = std::vector<float>(d_input_size);

        d_poly_gen = std::make_unique<aff3ct::tools::BCH_polynomial_generator<B_8>>(d_input_size, t);
        d_zeros = d_input_size - codeword;

        d_decoder = std::make_unique<aff3ct::module::Decoder_BCH_std<B_8, Q_8>>(d_frame_size, d_input_size, *d_poly_gen);
}

bch_decoder_impl::~bch_decoder_impl()
{
}

int bch_decoder_impl::get_output_size() { return d_output_size; }
int bch_decoder_impl::get_input_size() { return d_input_size; }

bool bch_decoder_impl::set_frame_size(unsigned int codeword)
{
    bool ret = true;
    if (codeword > d_max_frame_size) {
        d_logger->info("tried to set frame to {:d}; max possible is {:d}",
                    codeword,
                    d_max_frame_size);
        codeword = d_max_frame_size;
        ret = false;
    }

    uint8_t m = std::ceil(std::log2(codeword+1));
    d_input_size = std::pow(2,m)-1;
    d_frame_size = d_input_size - m*d_t;
    d_output_size = d_frame_size;

    return ret;
}

double bch_decoder_impl::rate() { return d_output_size / d_input_size; } // decoder rate

void bch_decoder_impl::generic_work(const void* inbuffer, void* outbuffer)
{
    const float* in = (const float*)inbuffer;
    B_8* out = (B_8*)outbuffer;

    volk_32f_s32f_multiply_32f(d_tmp_input.data(), in, -1.0f, d_input_size);
    d_quant->process(d_tmp_input.data(), d_quant_input.data(), -1);
    d_decoder->decode_siho(d_quant_input.data(), out, -1);

    if (d_zeros > 0) {
        ;
    }
}


} /* namespace fec_dev */
} /* namespace gr */
