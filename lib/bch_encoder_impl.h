/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_BCH_ENCODER_IMPL_H
#define INCLUDED_FEC_DEV_BCH_ENCODER_IMPL_H

#include <gnuradio/fec_dev/bch_encoder.h>
#include <map>
#include <string>
#include <cmath>

#include "Tools/types.h"
#include "Module/Encoder/BCH/Encoder_BCH.hpp"
#include "Module/Encoder/BCH/Encoder_BCH_inter.hpp"
#include "Tools/Code/BCH/BCH_polynomial_generator.hpp"

namespace gr {
namespace fec_dev {
    
class FEC_API bch_encoder_impl : public bch_encoder
{
private:
  int d_frame_bits;
  int d_K;
  int d_N;
  int d_zeros;
  int d_codeword_size;
  std::vector<B_32> d_tmp_input;
  std::vector<B_32> d_tmp_output;

  std::unique_ptr<aff3ct::module::Encoder_BCH<B_32>> d_encoder;
  std::unique_ptr<aff3ct::tools::BCH_polynomial_generator<B_32>> d_poly_gen;

public:
  bch_encoder_impl(int frame_bits, uint8_t t=5, SIMD::simd_strat_t simd_strat=SIMD::SEQ);
  ~bch_encoder_impl() override;

  bool set_frame_size(unsigned int frame_bits) override;
  double rate() override;
  int get_output_size() override;
  int get_input_size() override;
  void generic_work(const void* inbuffer, void* outbuffer) override;
};

} // namespace fec_dev
} // namespace gr

#endif /* INCLUDED_FEC_DEV_BCH_ENCODER_IMPL_H */
