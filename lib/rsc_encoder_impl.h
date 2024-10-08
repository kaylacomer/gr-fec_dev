/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_RSC_ENCODER_IMPL_H
#define INCLUDED_FEC_DEV_RSC_ENCODER_IMPL_H

#include <gnuradio/fec_dev/rsc_encoder.h>
#include <map>
#include <string>

#include "Tools/types.h"
#include "Module/Encoder/RSC/Encoder_RSC_generic_sys.hpp"

namespace gr {
namespace fec_dev {
    
class FEC_API rsc_encoder_impl : public rsc_encoder
{
private:
  unsigned int d_K;
  int d_N;
  int d_trellis_size;
  std::unique_ptr<aff3ct::module::Encoder_RSC_generic_sys<B_8>> d_encoder;

public:
  rsc_encoder_impl(int K, std::vector<int> polys={013,015}, int trellis_size = 8, bool buffered=true);
  ~rsc_encoder_impl() override;

  bool set_frame_size(unsigned int frame_size) override;
  double rate() override;
  int get_output_size() override;
  int get_input_size() override;
  void generic_work(const void* inbuffer, void* outbuffer) override;
};

} // namespace fec_dev
} // namespace gr

#endif /* INCLUDED_FEC_DEV_RSC_ENCODER_IMPL_H */