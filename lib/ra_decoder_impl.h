/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_RA_DECODER_IMPL_H
#define INCLUDED_FEC_DEV_RA_DECODER_IMPL_H

#include <gnuradio/fec_dev/ra_decoder.h>
#include <map>
#include <string>

#include "Tools/types.h"
#include "Module/Decoder/RA/Decoder_RA.hpp"
#include "Tools/Interleaver/Interleaver_core.hpp"
#include "Module/Quantizer/Pow2/Quantizer_pow2_fast.hpp"

#include "aff3ct_quantizer_headers.h"
#include "aff3ct_interleaver_headers.h"

namespace gr {
namespace fec_dev {
    
class FEC_API ra_decoder_impl : public ra_decoder
{
private:
  unsigned int d_K;
  int d_N;
  std::vector<float> d_tmp_input;
  std::vector<Q_8> d_quant_input;
  std::unique_ptr<aff3ct::module::Decoder_RA<B_8, Q_8>> d_decoder;
  std::unique_ptr<aff3ct::tools::Interleaver_core<>> d_interleaver_core;
  std::unique_ptr<aff3ct::module::Interleaver<Q_8>> d_pi;
  std::unique_ptr<aff3ct::module::Quantizer_pow2_fast<float, Q_8>> d_quant;

public:
  ra_decoder_impl(int K, int rep=3, int iter=1);
  ~ra_decoder_impl() override;

  bool set_frame_size(unsigned int K) override;
  double rate() override;
  int get_output_size() override;
  int get_input_size() override;
  void generic_work(const void* inbuffer, void* outbuffer) override;
};

} // namespace fec_dev
} // namespace gr

#endif /* INCLUDED_FEC_DEV_RA_DECODER_IMPL_H */