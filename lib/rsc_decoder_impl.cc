/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/io_signature.h>
#include "rsc_decoder_impl.h"
#include <volk/volk.h>

namespace gr {
namespace fec_dev {

fec::generic_decoder::sptr rsc_decoder::make(int K)
{
    return fec::generic_decoder::sptr(new rsc_decoder_impl(K));
}
    rsc_decoder_impl::rsc_decoder_impl(int K)
        : generic_decoder("rsc_decoder"),
        d_K(K)
    {
        set_frame_size(K);

        d_quant = std::make_unique<aff3ct::module::Quantizer_pow2_fast<float, Q_8>>(d_N, 2);
        d_quant_input = std::vector<Q_8>(d_N);
        d_tmp_input = std::vector<float>(d_N);

        std::vector<int> polys={013, 015};
        bool is_closed = true;
        bool buffered = true;

        auto enco_n = aff3ct::module::Encoder_RSC_generic_sys<B_8>(d_K, d_N, buffered, polys);
        auto trellis_n = enco_n.get_trellis();

        d_decoder = std::make_unique<aff3ct::module::Decoder_Viterbi_SIHO<B_8, Q_8>>(d_K, trellis_n, is_closed);
}

rsc_decoder_impl::~rsc_decoder_impl()
{
}

int rsc_decoder_impl::get_output_size() { return d_N; }
int rsc_decoder_impl::get_input_size() { return d_K; }

bool rsc_decoder_impl::set_frame_size(unsigned int K)
{
    d_N = 2 * (d_K + std::log2(d_trellis_size));
    return true;
}

double rsc_decoder_impl::rate() { return static_cast<float>(d_K) / d_N; } // decoder rate

void rsc_decoder_impl::generic_work(const void* inbuffer, void* outbuffer)
{
    const float* in = (const float*)inbuffer;
    B_8* out = (B_8*)outbuffer;

    volk_32f_s32f_multiply_32f(d_tmp_input.data(), in, -1.0f, d_N);
    d_quant->process(d_tmp_input.data(), d_quant_input.data(), -1);
    
    // std::vector<B_8> temp_output(d_K);
    int status = d_decoder->decode_siho(d_quant_input.data(), out, -1);
    if (status == 1) {
        std::cout << "Decoding FAILURE" << std::endl;
    }

    // std::memcpy(out, temp_output.data(), d_K * sizeof(B_8));
}

} /* namespace fec_dev */
} /* namespace gr */