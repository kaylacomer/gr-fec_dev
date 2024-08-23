#include "Module/Encoder/RA/Encoder_RA.hpp"/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_RA_ENCODER_IMPL_H
#define INCLUDED_FEC_DEV_RA_ENCODER_IMPL_H

#include <gnuradio/fec_dev/ra_encoder.h>
#include <map>
#include <string>

#include "Tools/types.h"
#include "Module/Encoder/RA/Encoder_RA.hpp"
#include "Tools/Interleaver/Interleaver_core.hpp"

#include "aff3ct_interleaver_headers.h"

namespace gr {
namespace fec_dev {
    
class FEC_API ra_encoder_impl : public ra_encoder
{
private:
  unsigned int d_K;
  int d_N;
  std::unique_ptr<aff3ct::module::Encoder_RA<B_8>> d_encoder;
  std::unique_ptr<aff3ct::tools::Interleaver_core<>> d_interleaver_core;
  std::unique_ptr<aff3ct::module::Interleaver<B_8>> d_interleaver;

public:
  ra_encoder_impl(int K, int rep=3);
  ~ra_encoder_impl() override;

  bool set_frame_size(unsigned int K) override;
  double rate() override;
  int get_output_size() override;
  int get_input_size() override;
  void generic_work(const void* inbuffer, void* outbuffer) override;
};

} // namespace fec_dev
} // namespace gr

#endif /* INCLUDED_FEC_DEV_RA_ENCODER_IMPL_H */