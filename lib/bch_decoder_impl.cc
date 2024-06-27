/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/fec_dev/bch_decoder.h>
#include <gnuradio/io_signature.h>
#include "bch_decoder_impl.h"

namespace gr {
namespace fec_dev {

fec::generic_decoder::sptr bch_decoder::make(int frame_size)
{
    return fec::generic_decoder::sptr(new bch_decoder_impl(frame_size));
}
    bch_decoder_impl::bch_decoder_impl(int frame_size)
        : generic_decoder("bch_decoder"),
        d_max_frame_size(frame_size)
    {
        set_frame_size(frame_size);

        int N = 63;
        int t = 3;
        d_poly_gen = std::make_unique<aff3ct::tools::BCH_polynomial_generator<B_8>>(N, t);
        int n_redundancy = d_poly_gen->get_n_rdncy();
        int K = N - n_redundancy;

        d_input_size = N;
        d_output_size = K;

        d_decoder = std::make_unique<aff3ct::module::Decoder_BCH_std<B_8, Q_8>>(K, N, *d_poly_gen);
}

bch_decoder_impl::~bch_decoder_impl()
{
}

int bch_decoder_impl::get_output_size() { return d_output_size; }
int bch_decoder_impl::get_input_size() { return d_input_size; }

bool bch_decoder_impl::set_frame_size(unsigned int frame_size)
{
    bool ret = true;
    if (frame_size > d_max_frame_size) {
        d_logger->info("tried to set frame to {:d}; max possible is {:d}",
                    frame_size,
                    d_max_frame_size);
        frame_size = d_max_frame_size;
        ret = false;
    }

    d_frame_size = frame_size;

    return ret;
}

double bch_decoder_impl::rate() { return d_output_size / d_input_size; } // decoder rate

void bch_decoder_impl::generic_work(const void* inbuffer, void* outbuffer)
{
    const float* in = (const float*)inbuffer;
    B_8* out = (B_8*)outbuffer;

    auto quant_input =  std::vector<Q_8>(d_input_size);
    auto quantizer = aff3ct::module::Quantizer_pow2_fast<float,Q_8>(d_input_size, 6);
    quantizer.process(in, quant_input.data(), -1);

    d_decoder->decode_siho(quant_input.data(), out, -1);
}


} /* namespace fec_dev */
} /* namespace gr */
