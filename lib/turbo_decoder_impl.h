/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FEC_TURBO_DECODER_IMPL_H
#define INCLUDED_FEC_TURBO_DECODER_IMPL_H

#include <gnuradio/fec_dev/turbo_decoder.h>
#include "gnuradio/fec_dev/aff3ct_decoder.h"
#include "gnuradio/fec_dev/aff3ct_interleaver.h"
#include <volk/volk.h>

#include <map>
#include <string>
#include <cmath>
#include <cstdio>
#include <sstream>
#include <vector>

#include "Tools/types.h"
#include "Module/Quantizer/Quantizer.hpp"
#include "Module/Quantizer/Pow2/Quantizer_pow2_fast.hpp"
#include "Module/Encoder/RSC/Encoder_RSC_generic_sys.hpp"
#include "Module/Decoder/Turbo/Decoder_turbo.hpp"
#include "Module/Decoder/Turbo/Decoder_turbo_fast.hpp"
#include "Module/Decoder/RSC/BCJR/Seq/Decoder_RSC_BCJR_seq_fast.hpp"
#include "Module/Decoder/RSC/BCJR/Seq_generic/Decoder_RSC_BCJR_seq_generic_std.hpp"

namespace gr {
namespace fec_dev {

class FEC_API turbo_decoder_impl : public turbo_decoder
{
private:
    int get_output_size() override;
    int get_input_size() override;
    int get_input_item_size() override;
    const char* get_input_conversion() override;

    int d_K;
    int d_N;
    int d_trellis_size;
    std::vector<float> d_tmp_input;
    std::vector<Q_8> d_quant_input;
    std::unique_ptr<aff3ct::module::Decoder_turbo<B_8, Q_8>> d_decoder;
    std::unique_ptr<aff3ct::tools::Interleaver_core<>> d_interleaver_core;
    std::unique_ptr<aff3ct::module::Interleaver<Q_8>> d_pi;
    std::unique_ptr<aff3ct::module::Quantizer_pow2_fast<float, Q_8>> d_quant;

public:
    turbo_decoder_impl(int frame_bits,
                    int n_iterations = 6,
                    Turbo::enc_standard_t standard=Turbo::LTE,
                    bool buffered=true,
                    std::vector<int> polys={013,015},
                    int trellis_size = 8,
                    Quantizer::quantizer_impl_t quant_impl=Quantizer::STD,
                    Turbo::subenc_implem_t subenc_impl=Turbo::sys,
                    int n_ff = -1,
                    Decoder::decoder_impl_t dec_impl=Decoder::STD,
                    BCJR::bcjr_impl_t bcjr_impl=BCJR::GENERIC,
                    SIMD::simd_strat_t simd_strat=SIMD::SEQ,
                    SIMD::simd_interintra_impl_t simd_interintra_impl=SIMD::NA,
                    Interleaver::itl_read_order_t read_order=Interleaver::NA,
                    int itl_n_cols = -1);
    ~turbo_decoder_impl() override;

    bool set_frame_size(unsigned int frame_bits) override;
    double rate() override;
    void generic_work(const void* inbuffer, void* outbuffer) override;
};

} /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_turbo_DECODER_IMPL_H */
