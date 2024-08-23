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
  tpc_decoder_aff3ct_impl(int K);
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