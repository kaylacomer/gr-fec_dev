/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/fec_dev/rs_decoder.h>
#include <gnuradio/io_signature.h>
#include "rs_decoder_impl.h"
#include <volk/volk.h>

namespace gr {
namespace fec_dev {

fec::generic_decoder::sptr rs_decoder::make(int frame_size)
{
    return fec::generic_decoder::sptr(std::make_shared<rs_decoder_impl>(frame_size));
}
    rs_decoder_impl::rs_decoder_impl(int frame_size)
        : generic_decoder("rs_decoder"),
        d_frame_size(frame_size)
    {
        d_t=5;
        set_frame_size(frame_size);

        d_quant = std::make_unique<aff3ct::module::Quantizer_pow2_fast<float, Q_8>>(d_N, 2);
        d_quant_input = std::vector<Q_8>(d_N);
        d_tmp_input = std::vector<float>(d_N);

        d_poly_gen = std::make_unique<aff3ct::tools::RS_polynomial_generator>(d_N, d_t);

        d_decoder = std::make_unique<aff3ct::module::Decoder_RS_std<B_8, Q_8>>(d_K, d_N, *d_poly_gen);
}

rs_decoder_impl::~rs_decoder_impl()
{
}

int rs_decoder_impl::get_output_size() { return d_frame_size; }
int rs_decoder_impl::get_input_size() { return d_N - (d_K - d_frame_size); }

bool rs_decoder_impl::set_frame_size(unsigned int frame_size)
{
    bool ret = true;
    
    uint8_t m = std::ceil(std::log2(frame_size+1));
    d_N = std::pow(2,m)-1;
    d_K = d_N - 2*d_t;

    return ret;
}

double rs_decoder_impl::rate() { return static_cast<float>((d_N - (d_K - d_frame_size))) / d_frame_size; } // decoder rate

void rs_decoder_impl::generic_work(const void* inbuffer, void* outbuffer)
{
    // const float* in = (const float*)inbuffer;
    // B_8* out = (B_8*)outbuffer;

    // volk_32f_s32f_multiply_32f(d_tmp_input.data(), in, -1.0f, d_N);
    // d_quant->process(d_tmp_input.data(), d_quant_input.data(), -1);

    // int zeros = d_K - d_frame_size;
    // if (zeros > 0) {
    //     d_quant_input.insert(d_quant_input.begin(), zeros, 0);
    // }
    
    // std::vector<B_8> tmp_output(d_K);
    // int status = d_decoder->decode_siho(d_quant_input.data(), tmp_output.data(), -1);

    // std::memcpy(out, tmp_output.data(), d_frame_size * sizeof(B_8));
}

} /* namespace fec_dev */
} /* namespace gr */
