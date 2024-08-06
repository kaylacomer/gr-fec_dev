/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_RSC_DECODER_IMPL_H
#define INCLUDED_FEC_DEV_RSC_DECODER_IMPL_H

#include <gnuradio/fec_dev/rsc_decoder.h>
#include <map>
#include <string>

#include "Tools/types.h"
// #include "Module/Encoder/RSC/Encoder_rsc.hpp"

namespace gr {
namespace fec_dev {
    
class FEC_API rsc_decoder_impl : public rsc_decoder
{
private:
  unsigned int d_K;
  int d_N;
  //std::unique_ptr<aff3ct::module::Encoder_RA<B_8>> d_decoder;

public:
  rsc_decoder_impl(int K);
  ~rsc_decoder_impl() override;

  bool set_frame_size(unsigned int frame_size) override;
  double rate() override;
  int get_output_size() override;
  int get_input_size() override;
  void generic_work(const void* inbuffer, void* outbuffer) override;
};

} // namespace fec_dev
} // namespace gr

#endif /* INCLUDED_FEC_DEV_RSC_DECODER_IMPL_H */