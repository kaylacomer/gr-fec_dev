/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_TURBO_ENCODER_IMPL_H
#define INCLUDED_FEC_DEV_TURBO_ENCODER_IMPL_H

#include <map>
#include <string>
#include <sstream>
#include <vector>

#include <gnuradio/fec_dev/turbo_encoder.h>
#include "gnuradio/fec_dev/aff3ct_interleaver.h"

#include "Tools/types.h"
#include "Module/Encoder/RSC/Encoder_RSC_generic_sys.hpp"
#include "Module/Encoder/Turbo/Encoder_turbo.hpp"

#include "aff3ct_interleaver_headers.h"

namespace gr {
namespace fec_dev {
    
class FEC_API turbo_encoder_impl : public turbo_encoder
{
private:
  int d_K;
  int d_N;
  int d_trellis_size;
  std::unique_ptr<aff3ct::module::Encoder_turbo<B_8>> d_encoder;
  std::unique_ptr<aff3ct::tools::Interleaver_core<>> d_interleaver_core;
  std::unique_ptr<aff3ct::module::Interleaver<B_8>> d_pi;

public:
  turbo_encoder_impl(int frame_bits,
                    Turbo::enc_standard_t standard=Turbo::LTE,
                    bool buffered=true,
                    std::vector<int> polys={013,015},
                    int trellis_size = 8,
                    Interleaver::interleaver_t interleaver=Interleaver::NO,
                    Interleaver::itl_read_order_t read_order=Interleaver::NA,
                    int itl_n_cols = -1);
  ~turbo_encoder_impl() override;

  bool set_frame_size(unsigned int frame_bits) override;
  double rate() override;
  int get_output_size() override;
  int get_input_size() override;
  void generic_work(const void* inbuffer, void* outbuffer) override;
};

} // namespace fec_dev
} // namespace gr

#endif /* INCLUDED_FEC_DEV_TURBO_ENCODER_IMPL_H */
