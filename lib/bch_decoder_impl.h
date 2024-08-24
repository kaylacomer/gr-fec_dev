/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_BCH_DECODER_IMPL_H
#define INCLUDED_FEC_DEV_BCH_DECODER_IMPL_H

#include <gnuradio/fec_dev/bch_decoder.h>
#include <map>
#include <string>
#include <cmath>

#include "Tools/types.h"
#include "Module/Decoder/BCH/Standard/Decoder_BCH_std.hpp"
#include "Module/Decoder/BCH/Fast/Decoder_BCH_fast.hpp"
#include "Module/Decoder/BCH/Genius/Decoder_BCH_genius.hpp"
#include "Module/Decoder/BCH/Decoder_BCH.hpp"
#include "Module/Encoder/BCH/Encoder_BCH.hpp"
#include "Module/Encoder/BCH/Encoder_BCH_inter.hpp"
#include "Tools/Code/BCH/BCH_polynomial_generator.hpp"
#include "Module/Quantizer/Pow2/Quantizer_pow2_fast.hpp"
#include "aff3ct_quantizer_headers.h"

namespace gr {
namespace fec_dev {
    
class FEC_API bch_decoder_impl : public bch_decoder
{
private:
  unsigned int d_frame_size;
  int d_K;
  int d_N;
  int d_zeros;
  int d_codeword_size;
  std::vector<float> d_tmp_input;
  std::vector<B_8> d_tmp_output;
  std::vector<Q_8> d_quant_input;

  std::unique_ptr<aff3ct::module::Decoder_BCH<B_8, Q_8>> d_decoder;
  std::unique_ptr<aff3ct::tools::BCH_polynomial_generator<B_8>> d_poly_gen;
  std::unique_ptr<aff3ct::module::Quantizer<float, Q_8>> d_quant;

public:
  bch_decoder_impl(int frame_bits, uint8_t t=5, uint8_t quant_fixed_point_pos = 2, uint8_t quant_saturation_pos = 6,
                    Quantizer::quantizer_impl_t quant_impl=Quantizer::STD, Decoder::decoder_impl_t dec_impl=Decoder::STD,
                    SIMD::simd_strat_t simd_strat=SIMD::SEQ);
  ~bch_decoder_impl() override;

  bool set_frame_size(unsigned int frame_bits) override;
  double rate() override;
  int get_output_size() override;
  int get_input_size() override;
  void generic_work(const void* inbuffer, void* outbuffer) override;
};

} // namespace fec_dev
} // namespace gr

#endif /* INCLUDED_FEC_DEV_BCH_DECODER_IMPL_H */