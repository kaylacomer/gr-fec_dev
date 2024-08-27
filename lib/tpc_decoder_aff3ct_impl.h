/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_TPC_DECODER_AFF3CT_IMPL_H
#define INCLUDED_FEC_DEV_TPC_DECODER_AFF3CT_IMPL_H

#include <gnuradio/fec_dev/tpc_decoder_aff3ct.h>
#include <map>
#include <string>

#include "Tools/types.h"
#include "Tools/Interleaver/Interleaver_core.hpp"
#include "Tools/Interleaver/Row_column/Interleaver_core_row_column.hpp"
#include "Module/Encoder/BCH/Encoder_BCH.hpp"
#include "Tools/Code/BCH/BCH_polynomial_generator.hpp"
#include "Module/Decoder/Turbo_product/Decoder_turbo_product.hpp"
#include "Module/Decoder/Turbo_product/Chase_pyndiah/Decoder_chase_pyndiah.hpp"
#include "Module/Decoder/BCH/Standard/Decoder_BCH_std.hpp"
#include "Module/Quantizer/Quantizer.hpp"
#include "Module/Quantizer/Pow2/Quantizer_pow2_fast.hpp"

#include "aff3ct_quantizer_headers.h"
#include "aff3ct_interleaver_headers.h"

namespace gr {
namespace fec_dev {
    
class FEC_API tpc_decoder_aff3ct_impl : public tpc_decoder_aff3ct
{
private:
  int d_K;
  int d_N;
  int d_t;
  std::vector<float> d_tmp_input;
  std::vector<Q_8> d_quant_input;

  std::unique_ptr<aff3ct::module::Decoder_turbo_product<B_8, Q_8>> d_decoder;
  std::unique_ptr<aff3ct::tools::BCH_polynomial_generator<B_8>> d_poly_gen;
  std::unique_ptr<aff3ct::tools::Interleaver_core<>> d_interleaver_core;
  std::unique_ptr<aff3ct::module::Interleaver<Q_8>> d_pi;
  std::unique_ptr<aff3ct::module::Quantizer_pow2_fast<float, Q_8>> d_quant;

public:
  tpc_decoder_aff3ct_impl(int K_sqrt,
                          int N_sqrt,
                          int t,
                          SIMD::simd_strat_t bch_simd_strat=SIMD::SEQ,
                          Interleaver::interleaver_t interleaver=Interleaver::ROW_COL,
                          Interleaver::itl_read_order_t read_order=Interleaver::TOP_LEFT,
                          bool parity_extended=false,
                          uint8_t quant_fixed_point_pos = 2,
                          uint8_t quant_saturation_pos = 6,
                          Quantizer::quantizer_impl_t quant_impl=Quantizer::STD,
                          Decoder::decoder_impl_t bch_dec_impl=Decoder::STD,
                          Decoder::decoder_impl_t chase_pyndiah_impl=Decoder::STD,
                          int n_iterations = 4,
                          std::vector<float> alpha = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5},
                          std::vector<float> beta = {},
                          std::vector<float> chase_pyndiah_coef = {1, 1, 1, 1, 0},
                          int n_least_reliable_pos = 2,
                          int n_test_vectors = 4,
                          int n_competitors = 4
                          );
  ~tpc_decoder_aff3ct_impl() override;

  bool set_frame_size(unsigned int frame_size) override;
  double rate() override;
  int get_output_size() override;
  int get_input_size() override;
  void generic_work(const void* inbuffer, void* outbuffer) override;
};

} // namespace fec_dev
} // namespace gr

#endif /* INCLUDED_FEC_DEV_TPC_DECODER_AFF3CT_IMPL_H */