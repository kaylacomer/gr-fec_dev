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

#include "Tools/types.h"
#include "Module/Encoder/BCH/Encoder_bch.hpp"

namespace gr {
namespace fec_dev {
    
class FEC_API bch_encoder_impl : public bch_encoder
{
private:
  unsigned int d_frame_size;
  unsigned int d_max_frame_size;
  int d_output_size;

public:
  bch_encoder_impl(int frame_size);
  ~bch_encoder_impl() override;

  bool set_frame_size(unsigned int frame_size) override;
  double rate() override;
  int get_output_size() override;
  int get_input_size() override;
  void generic_work(const void* inbuffer, void* outbuffer) override;
};

} // namespace fec_dev
} // namespace gr

#endif /* INCLUDED_FEC_DEV_BCH_ENCODER_IMPL_H */
