/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <Module/Quantizer/Pow2/Quantizer_pow2.hpp>
#include <Module/Quantizer/Pow2/Quantizer_pow2_fast.hpp>

#include "gnuradio/fec_dev/aff3ct_common.h"

#include "Module/Decoder/RSC/BCJR/Seq/Decoder_RSC_BCJR_seq_fast.hpp"
#include "Module/Decoder/RSC/BCJR/Seq_generic/Decoder_RSC_BCJR_seq_generic_std.hpp"

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
                                               interleaver_t standard,
                                               enc_sub_type_t subencoder,
                                               bool buffered,
                                               std::vector<int> polys,
                                               int trellis_size,
                                               int n_iterations)
{
    return fec::generic_decoder::sptr(new turbo_decoder_impl(
        frame_size, standard, subencoder, buffered, polys, trellis_size, n_iterations));
}

turbo_decoder_impl::turbo_decoder_impl(int frame_size,
                                       interleaver_t standard,
                                       enc_sub_type_t subencoder,
                                       bool buffered,
                                       std::vector<int> polys,
                                       int trellis_size,
                                       int n_iterations)
    : generic_decoder("turbo_decoder"),
      d_frame_size(frame_size),
      d_trellis_size(trellis_size)
{
    if (standard == LTE) {
        d_trellis_size = 8;
        polys = {013,015};

        d_K = d_frame_size;

        d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_LTE<>>(d_K);
    }
    else if (standard == CCSDS) {
        d_trellis_size = 16;
        polys = {023,033};

        std::vector<int> CCSDS_sizes = {1784, 3568, 7136, 8920};
        for (const int& size : CCSDS_sizes) {
            if (frame_size <= size) {
                d_K = size;
                break;
            }
        }

        d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_CCSDS<>>(d_K);
    }
    else {
        d_K = d_frame_size;
        d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_random<>>(d_K);
    }

    d_N = 3 * d_K + 4 * std::log2(d_trellis_size);

    d_pi = std::make_unique<aff3ct::module::Interleaver<Q_8>>(*d_interleaver_core);

    d_quant = std::make_unique<aff3ct::module::Quantizer_pow2_fast<float, Q_8>>(d_N, 2);
    d_quant_input = std::vector<Q_8>(d_N);
    d_tmp_input = std::vector<float>(d_N);

    int N_rsc = 2 * (d_K + std::log2(d_trellis_size));
    auto enco_n = aff3ct::module::Encoder_RSC_generic_sys<B_8>(d_K, N_rsc, buffered, polys);
    auto enco_i = enco_n;

    auto trellis_n = enco_n.get_trellis();
    auto trellis_i = trellis_n;

    auto dec_n = aff3ct::module::Decoder_RSC_BCJR_seq_generic_std<B_8, Q_8>(d_K, trellis_n, buffered);
    auto dec_i = aff3ct::module::Decoder_RSC_BCJR_seq_generic_std<B_8, Q_8>(d_K, trellis_i, buffered);

    d_decoder = std::make_unique<aff3ct::module::Decoder_turbo_fast<B_8, Q_8>>(d_K, d_N, n_iterations, dec_n, dec_i, *d_pi, buffered);
}

turbo_decoder_impl::~turbo_decoder_impl() {}

int turbo_decoder_impl::get_output_size() { return d_frame_size; }

int turbo_decoder_impl::get_input_size() { return d_N; }

int turbo_decoder_impl::get_input_item_size() { return sizeof(float); }

const char* turbo_decoder_impl::get_input_conversion() { return "none"; }

bool turbo_decoder_impl::set_frame_size(unsigned int frame_size)
{
    return true;
}

double turbo_decoder_impl::rate() { return static_cast<float>(d_frame_size) / d_N; }

void turbo_decoder_impl::generic_work(const void* inbuffer, void* outbuffer)
{
    const float* in = (const float*)inbuffer;
    B_8* out = (B_8*)outbuffer;

    volk_32f_s32f_multiply_32f(d_tmp_input.data(), in, -1.0f, d_N);
    d_quant->process(d_tmp_input.data(), d_quant_input.data(), -1);
    
    std::vector<B_8> temp_output(d_K);
    int status = d_decoder->decode_siho(d_quant_input.data(), temp_output.data(), -1);
    if (status == 1) {
        std::cout << "Decoding FAILURE" << std::endl;
    }

    std::memcpy(out, temp_output.data(), d_frame_size * sizeof(B_8));
}

} // namespace fec_dev
} /* namespace gr */
