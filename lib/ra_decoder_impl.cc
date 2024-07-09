/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/fec_dev/ra_decoder.h>
#include <gnuradio/io_signature.h>
#include "ra_decoder_impl.h"
#include "Tools/Interleaver/NO/Interleaver_core_NO.hpp"
#include <volk/volk.h>

namespace gr {
namespace fec_dev {

fec::generic_decoder::sptr ra_decoder::make(int K, int N, int iter)
{
    return fec::generic_decoder::sptr(new ra_decoder_impl(K, N, iter));
}
    ra_decoder_impl::ra_decoder_impl(int K, int N, int iter)
        : generic_decoder("ra_decoder"),
        d_K(K),
        d_N(N)
    {
        set_frame_size(K);

        d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_NO<>>(d_N);
        d_pi = std::make_unique<aff3ct::module::Interleaver<Q_8>>(*d_interleaver_core);

        d_quant = std::make_unique<aff3ct::module::Quantizer_pow2_fast<float, Q_8>>(d_N, 2);
        d_quant_input = std::vector<Q_8>(d_N);
        d_tmp_input = std::vector<float>(d_N);

        d_decoder = std::make_unique<aff3ct::module::Decoder_RA<B_8, Q_8>>(d_K, d_N, *d_pi, iter);
}

ra_decoder_impl::~ra_decoder_impl()
{
}

int ra_decoder_impl::get_output_size() { return d_K; }
int ra_decoder_impl::get_input_size() { return d_N; }

bool ra_decoder_impl::set_frame_size(unsigned int K)
{
    return true;
}

double ra_decoder_impl::rate() { return d_K / d_N; } // decoder rate

void ra_decoder_impl::generic_work(const void* inbuffer, void* outbuffer)
{
    const float* in = (const float*)inbuffer;
    B_8* out = (B_8*)outbuffer;

    volk_32f_s32f_multiply_32f(d_tmp_input.data(), in, -1.0f, d_N);
    d_quant->process(d_tmp_input.data(), d_quant_input.data(), -1);
    
    d_decoder->decode_siho(d_quant_input.data(), out, -1);
}

} /* namespace fec_dev */
} /* namespace gr */
