/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_RS_ENCODER_IMPL_H
#define INCLUDED_FEC_DEV_RS_ENCODER_IMPL_H

#include <gnuradio/fec_dev/rs_encoder.h>
#include <map>
#include <string>
#include <memory>

#include "Tools/types.h"
#include "Module/Encoder/RS/Encoder_RS.hpp"
#include "Tools/Code/RS/RS_polynomial_generator.hpp"

namespace gr {
namespace fec_dev {
    
class FEC_API rs_encoder_impl : public rs_encoder
{
private:
  unsigned int d_frame_size;
  int d_K;
  int d_N;
  int d_zeros;
  int d_codeword_size;
  std::vector<B_8> d_tmp_input;
  std::vector<B_8> d_tmp_output;
  std::unique_ptr<aff3ct::module::Encoder_RS<B_8>> d_encoder;
  std::unique_ptr<aff3ct::tools::RS_polynomial_generator> d_poly_gen;

public:
  rs_encoder_impl(int frame_size, uint8_t t=5);
  ~rs_encoder_impl() override;

  bool set_frame_size(unsigned int frame_size) override;
  double rate() override;
  int get_output_size() override;
  int get_input_size() override;
  void generic_work(const void* inbuffer, void* outbuffer) override;
};

} // namespace fec_dev
} // namespace gr

#endif /* INCLUDED_FEC_DEV_RS_ENCODER_IMPL_H */