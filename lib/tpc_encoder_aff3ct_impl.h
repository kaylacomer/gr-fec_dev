/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_TPC_ENCODER_AFF3CT_IMPL_H
#define INCLUDED_FEC_DEV_TPC_ENCODER_AFF3CT_IMPL_H

#include <gnuradio/fec_dev/tpc_encoder_aff3ct.h>
#include <map>
#include <string>

#include "Tools/types.h"
#include "Module/Encoder/Turbo_product/Encoder_turbo_product.hpp"
#include "Tools/Interleaver/Interleaver_core.hpp"
#include "Tools/Interleaver/Row_column/Interleaver_core_row_column.hpp"
#include "Module/Encoder/BCH/Encoder_BCH.hpp"
#include "Tools/Code/BCH/BCH_polynomial_generator.hpp"

#include "aff3ct_interleaver_headers.h"

namespace gr {
namespace fec_dev {
    
class FEC_API tpc_encoder_aff3ct_impl : public tpc_encoder_aff3ct
{
private:
  unsigned int d_K_sqrt;
  int d_N_sqrt;
  std::unique_ptr<aff3ct::module::Encoder_turbo_product<B_8>> d_encoder;
  std::unique_ptr<aff3ct::tools::Interleaver_core<>> d_interleaver_core;
  std::unique_ptr<aff3ct::module::Interleaver<B_8>> d_pi;
  std::unique_ptr<aff3ct::tools::BCH_polynomial_generator<B_8>> d_poly_gen;

public:
  tpc_encoder_aff3ct_impl(int K_sqrt,
                          int N_sqrt,
                          int t,
                          SIMD::simd_strat_t bch_simd_strat=SIMD::SEQ,
                          Interleaver::interleaver_t interleaver=Interleaver::ROW_COL,
                          Interleaver::itl_read_order_t read_order=Interleaver::TOP_LEFT,
                          bool parity_extended=false
                          );
  ~tpc_encoder_aff3ct_impl() override;

  bool set_frame_size(unsigned int frame_size) override;
  double rate() override;
  int get_output_size() override;
  int get_input_size() override;
  void generic_work(const void* inbuffer, void* outbuffer) override;
};

} // namespace fec_dev
} // namespace gr

#endif /* INCLUDED_FEC_DEV_TPC_ENCODER_AFF3CT_IMPL_H */