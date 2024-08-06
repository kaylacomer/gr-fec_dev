/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_REP_ENCODER_IMPL_H
#define INCLUDED_FEC_DEV_REP_ENCODER_IMPL_H

#include <gnuradio/fec_dev/rep_encoder.h>
#include <map>
#include <string>

#include "Tools/types.h"
#include "Module/Encoder/Repetition/Encoder_repetition_sys.hpp"

namespace gr {
namespace fec_dev {
    
class FEC_API rep_encoder_impl : public rep_encoder
{
private:
  unsigned int d_K;
  int d_N;
  std::unique_ptr<aff3ct::module::Encoder_repetition_sys<B_8>> d_encoder;

public:
  rep_encoder_impl(int K, int rep=3);
  ~rep_encoder_impl() override;

  bool set_frame_size(unsigned int K) override;
  double rate() override;
  int get_output_size() override;
  int get_input_size() override;
  void generic_work(const void* inbuffer, void* outbuffer) override;
};

} // namespace fec_dev
} // namespace gr

#endif /* INCLUDED_FEC_DEV_REP_ENCODER_IMPL_H */