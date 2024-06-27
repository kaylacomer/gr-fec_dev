/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "turbo_decoder_impl.h"
#include <volk/volk.h>
#include <cmath>
#include <cstdio>
#include <sstream>
#include <vector>

namespace gr {
namespace fec_dev {

fec::generic_decoder::sptr turbo_decoder::make(int frame_size,
                                               enc_standard_t standard,
                                               enc_sub_type_t subencoder,
                                               bool buffered,
                                               std::vector<int> polys,
                                               int trellis_size,
                                               int n_iterations)
{
    return fec::generic_decoder::sptr(new turbo_decoder_impl(frame_size, standard, subencoder, buffered, polys, trellis_size, n_iterations));
}

turbo_decoder_impl::turbo_decoder_impl(int frame_size,
                                       enc_standard_t standard,
                                       enc_sub_type_t subencoder,
                                       bool buffered,
                                       std::vector<int> polys,
                                       int trellis_size,
                                       int n_iterations)
    : generic_decoder("turbo_decoder"),
    d_max_frame_size(frame_size),
    d_trellis_size(trellis_size)
{
    set_frame_size(frame_size);

    d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_LTE<>>(frame_size);
    d_pi = std::make_unique<aff3ct::module::Interleaver<B_8>>(*d_interleaver_core);

    int N_rsc = 2 * (frame_size+std::log2(trellis_size));
    auto enco_n = aff3ct::module::Encoder_RSC_generic_sys<B_8>(frame_size, N_rsc, buffered, polys);
    auto enco_i = enco_n;

    auto trellis_n = enco_n.get_trellis();
    auto trellis_i = trellis_n;
    
    auto dec_n = aff3ct::module::Decoder_RSC_BCJR_seq_fast<B_8, Q_8>(frame_size, trellis_n, buffered);
    auto dec_i = aff3ct::module::Decoder_RSC_BCJR_seq_fast<B_8, Q_8>(frame_size, trellis_i, buffered);

    d_decoder = std::make_unique<aff3ct::module::Decoder_turbo_fast<B_8, Q_8>>(frame_size, d_input_size, n_iterations, dec_n, dec_i, *d_pi, buffered);
}

turbo_decoder_impl::~turbo_decoder_impl() {}

int turbo_decoder_impl::get_output_size() { return d_frame_size; }

int turbo_decoder_impl::get_input_size() { return d_input_size; }

int turbo_decoder_impl::get_input_item_size() { return sizeof(float); }

const char* turbo_decoder_impl::get_input_conversion() { return "none"; }

bool turbo_decoder_impl::set_frame_size(unsigned int frame_size)
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
    d_output_size = frame_size;
    d_input_size = 3*d_frame_size + 4*std::log2(d_trellis_size);

    return ret;
}

double turbo_decoder_impl::rate() { return d_output_size / d_input_size; }

void turbo_decoder_impl::generic_work(const void* inbuffer, void* outbuffer)
{
    const float* in = (const float*)inbuffer;
    B_8* out = (B_8*)outbuffer;

    auto quant_input =  std::vector<Q_8>(d_input_size);
    auto quantizer = aff3ct::module::Quantizer_pow2_fast<float,Q_8>(d_input_size, 6);
    quantizer.process(in, quant_input.data(), -1);

    d_decoder->decode_siho(quant_input.data(), out, -1);
}

} /* namespace fec */
} /* namespace gr */
