/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_REP_DECODER_IMPL_H
#define INCLUDED_FEC_DEV_REP_DECODER_IMPL_H

#include <gnuradio/fec_dev/rep_decoder.h>
#include <map>
#include <string>

#include "Tools/types.h"
#include "Module/Decoder/Repetition/Decoder_repetition_std.hpp"
#include "Module/Decoder/Repetition/Decoder_repetition_fast.hpp"

#include "aff3ct_quantizer_headers.h"

namespace gr {
namespace fec_dev {
    
class FEC_API rep_decoder_impl : public rep_decoder
{
private:
  unsigned int d_K;
  int d_N;
  std::vector<float> d_tmp_input;
  std::vector<Q_8> d_quant_input;
  std::unique_ptr<aff3ct::module::Decoder_SISO<B_8, Q_8>> d_decoder;
  std::unique_ptr<aff3ct::module::Quantizer<float, Q_8>> d_quant;

public:
  rep_decoder_impl(int K, int rep=3, bool buffered = true, uint8_t quant_fixed_point_pos=2, uint8_t quant_saturation_pos=6,
  Quantizer::quantizer_impl_t quant_impl=Quantizer::STD, Decoder::decoder_impl_t dec_impl=Decoder::STD);
  ~rep_decoder_impl() override;

  bool set_frame_size(unsigned int K) override;
  double rate() override;
  int get_output_size() override;
  int get_input_size() override;
  void generic_work(const void* inbuffer, void* outbuffer) override;
};

} // namespace fec_dev
} // namespace gr

#endif /* INCLUDED_FEC_DEV_REP_DECODER_IMPL_H */