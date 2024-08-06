/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/io_signature.h>
#include "rep_decoder_impl.h"
#include <volk/volk.h>

namespace gr {
namespace fec_dev {

fec::generic_decoder::sptr rep_decoder::make(int K, int rep)
{
    return fec::generic_decoder::sptr(new rep_decoder_impl(K, rep));
}
    rep_decoder_impl::rep_decoder_impl(int K, int rep)
        : generic_decoder("rep_decoder"),
        d_K(K),
        d_N(rep * K)
    {
        set_frame_size(K);

        d_quant = std::make_unique<aff3ct::module::Quantizer_pow2_fast<float, Q_8>>(d_N, 2);
        d_quant_input = std::vector<Q_8>(d_N);
        d_tmp_input = std::vector<float>(d_N);

        d_decoder = std::make_unique<aff3ct::module::Decoder_repetition_std<B_8, Q_8>>(d_K, d_N);
}

rep_decoder_impl::~rep_decoder_impl()
{
}

int rep_decoder_impl::get_output_size() { return d_N; }
int rep_decoder_impl::get_input_size() { return d_K; }

bool rep_decoder_impl::set_frame_size(unsigned int K)
{
    return true;
}

double rep_decoder_impl::rate() { return static_cast<float>(d_K) / d_N; } // decoder rate

void rep_decoder_impl::generic_work(const void* inbuffer, void* outbuffer)
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
