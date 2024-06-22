/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_TURBO_ENCODER_IMPL_H
#define INCLUDED_FEC_DEV_TURBO_ENCODER_IMPL_H

#include <gnuradio/fec_dev/turbo_encoder.h>
#include <map>
#include <string>
#include "Module/Encoder/RSC/Encoder_RSC_generic_sys.hpp"
#include "Tools/Interleaver/Interleaver_core.hpp"
#include "Tools/Interleaver/LTE/Interleaver_core_LTE.hpp"
#include "Module/Encoder/Turbo/Encoder_turbo.hpp"

namespace gr {
namespace fec_dev {
    
class FEC_API turbo_encoder_impl : public turbo_encoder
{
private:
  unsigned int d_frame_size;
  unsigned int d_max_frame_size;
  int d_output_size;
  int d_trellis_size;
  std::shared_ptr<aff3ct::module::Encoder_RSC_generic_sys<>> d_enco_n;
  std::shared_ptr<aff3ct::module::Encoder_RSC_generic_sys<>> d_enco_i;
  std::shared_ptr<aff3ct::tools::Interleaver_core<>> d_interleaver_core;
  std::shared_ptr<aff3ct::module::Interleaver<int32_t>> d_pi;
  std::unique_ptr<aff3ct::module::Encoder_turbo<>> d_encoder;

public:
  turbo_encoder_impl(int frame_size,
                     enc_standard_t standard=LTE,
                     enc_sub_type_t subencoder=RSC,
                     bool buffered=true,
                     std::vector<int> polys={013,015},
                     int trellis_size = 8);
  ~turbo_encoder_impl() override;

  bool set_frame_size(unsigned int frame_size) override;
  double rate() override;
  int get_output_size() override;
  int get_input_size() override;
  void generic_work(const void* inbuffer, void* outbuffer) override;
};

} // namespace fec_dev
} // namespace gr

#endif /* INCLUDED_FEC_DEV_TURBO_ENCODER_IMPL_H */
