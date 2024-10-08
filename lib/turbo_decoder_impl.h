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
#include "Module/Decoder/Turbo/Decoder_turbo_std.hpp"
#include "Module/Decoder/RSC/BCJR/Seq/Decoder_RSC_BCJR_seq_fast.hpp"
#include "Module/Decoder/RSC/BCJR/Seq_generic/Decoder_RSC_BCJR_seq_generic_std.hpp"
#include "Module/Decoder/RSC/BCJR/Seq/Decoder_RSC_BCJR_seq_scan.hpp"
#include "Module/Decoder/RSC/BCJR/Seq/Decoder_RSC_BCJR_seq_std.hpp"
#include "Module/Decoder/RSC/BCJR/Seq/Decoder_RSC_BCJR_seq_fast.hpp"
#include "Module/Decoder/RSC/BCJR/Seq/Decoder_RSC_BCJR_seq_very_fast.hpp"
#include "Module/Decoder/RSC/BCJR/Inter/Decoder_RSC_BCJR_inter_fast.hpp"
#include "Module/Decoder/RSC/BCJR/Inter/Decoder_RSC_BCJR_inter_std.hpp"
#include "Module/Decoder/RSC/BCJR/Inter/Decoder_RSC_BCJR_inter_very_fast.hpp"
#include "Module/Decoder/RSC/BCJR/Intra/Decoder_RSC_BCJR_intra_fast.hpp"
#include "Module/Decoder/RSC/BCJR/Intra/Decoder_RSC_BCJR_intra_std.hpp"
#include "Module/Decoder/RSC/BCJR/Inter_intra/Decoder_RSC_BCJR_inter_intra_fast_x2_AVX.hpp"
#include "Module/Decoder/RSC/BCJR/Inter_intra/Decoder_RSC_BCJR_inter_intra_fast_x2_SSE.hpp"
#include "Module/Decoder/RSC/BCJR/Inter_intra/Decoder_RSC_BCJR_inter_intra_fast_x4_AVX.hpp"

#include "aff3ct_interleaver_headers.h"
#include "aff3ct_quantizer_headers.h"

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
    // std::unique_ptr<aff3ct::module::Decoder_RSC_BCJR<B_8, Q_8>> dec_n;
    // std::unique_ptr<aff3ct::module::Decoder_RSC_BCJR<B_8, Q_8>> dec_i;
    std::unique_ptr<aff3ct::module::Decoder_turbo<B_8, Q_8>> d_decoder;
    std::unique_ptr<aff3ct::tools::Interleaver_core<>> d_interleaver_core;
    std::unique_ptr<aff3ct::module::Interleaver<Q_8>> d_pi;
    std::unique_ptr<aff3ct::module::Quantizer<float, Q_8>> d_quant;

public:
    turbo_decoder_impl(int frame_bits,
                    int n_iterations = 6,
                    Turbo::enc_standard_t standard=Turbo::LTE,
                    bool buffered=true,
                    std::vector<int> polys={013,015},
                    int trellis_size = 8,
                    uint8_t quant_fixed_point_pos = 2,
                    uint8_t quant_saturation_pos = 6,
                    bool set_sat_pos = false,
                    Quantizer::quantizer_impl_t quant_impl=Quantizer::STD,
                    Decoder::decoder_impl_t dec_impl=Decoder::STD,
                    BCJR::bcjr_impl_t bcjr_impl=BCJR::GENERIC,
                    SIMD::simd_strat_t simd_strat=SIMD::SEQ,
                    SIMD::simd_interintra_impl_t simd_interintra_impl=SIMD::NA,
                    Interleaver::interleaver_t interleaver=Interleaver::NO,
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
