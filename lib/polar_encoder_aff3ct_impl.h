/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_POLAR_ENCODER_AFF3CT_IMPL_H
#define INCLUDED_FEC_DEV_POLAR_ENCODER_AFF3CT_IMPL_H

#include <gnuradio/fec_dev/polar_encoder_aff3ct.h>
#include <map>
#include <string>

#include "Tools/types.h"
//#include "Module/Encoder/POLAR/Encoder_polar.hpp"

namespace gr {
namespace fec_dev {
    
class FEC_API polar_encoder_aff3ct_impl : public polar_encoder_aff3ct
{
private:
  unsigned int d_K;
  int d_N;
  //std::unique_ptr<aff3ct::module::Encoder_RA<B_8>> d_encoder;

public:
  polar_encoder_aff3ct_impl(int K);
  ~polar_encoder_aff3ct_impl() override;

  bool set_frame_size(unsigned int frame_size) override;
  double rate() override;
  int get_output_size() override;
  int get_input_size() override;
  void generic_work(const void* inbuffer, void* outbuffer) override;
};

} // namespace fec_dev
} // namespace gr

#endif /* INCLUDED_FEC_DEV_POLAR_ENCODER_AFF3CT_IMPL_H */