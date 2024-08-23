/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_RS_DECODER_IMPL_H
#define INCLUDED_FEC_DEV_RS_DECODER_IMPL_H

#include <gnuradio/fec_dev/rs_decoder.h>
#include <map>
#include <string>

#include "Tools/types.h"
#include "Module/Decoder/RS/Standard/Decoder_RS_std.hpp"
#include "Tools/Code/RS/RS_polynomial_generator.hpp"
#include "Module/Quantizer/Pow2/Quantizer_pow2_fast.hpp"

#include "aff3ct_quantizer_headers.h"

namespace gr {
namespace fec_dev {
    
class FEC_API rs_decoder_impl : public rs_decoder
{
private:
  unsigned int d_frame_size;
  int d_K;
  int d_N;
  int d_t;
  std::vector<float> d_tmp_input;
  std::vector<Q_8> d_quant_input;

  std::unique_ptr<aff3ct::module::Decoder_RS_std<B_8, Q_8>> d_decoder;
  std::unique_ptr<aff3ct::tools::RS_polynomial_generator> d_poly_gen;
  std::unique_ptr<aff3ct::module::Quantizer_pow2_fast<float, Q_8>> d_quant;

public:
  rs_decoder_impl(int frame_size);
  ~rs_decoder_impl() override;

  bool set_frame_size(unsigned int frame_size) override;
  double rate() override;
  int get_output_size() override;
  int get_input_size() override;
  void generic_work(const void* inbuffer, void* outbuffer) override;
};

} // namespace fec_dev
} // namespace gr

#endif /* INCLUDED_FEC_DEV_RS_DECODER_IMPL_H */