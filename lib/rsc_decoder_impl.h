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
#include "Module/Quantizer/Quantizer.hpp"
#include "Module/Quantizer/Pow2/Quantizer_pow2_fast.hpp"
#include "Module/Decoder/RSC/Viterbi/Decoder_Viterbi_SIHO.hpp"
#include "Module/Decoder/RSC/BCJR/Seq/Decoder_RSC_BCJR_seq_fast.hpp"
#include "Module/Decoder/RSC/BCJR/Seq_generic/Decoder_RSC_BCJR_seq_generic_std.hpp"
#include "Module/Encoder/RSC/Encoder_RSC_generic_sys.hpp"

#include "aff3ct_quantizer_headers.h"

namespace gr {
namespace fec_dev {
    
class FEC_API rsc_decoder_impl : public rsc_decoder
{
private:
  unsigned int d_K;
  int d_N;
  int d_trellis_size;
  std::unique_ptr<aff3ct::module::Decoder_Viterbi_SIHO<B_8, Q_8>> d_decoder;
  std::vector<float> d_tmp_input;
  std::vector<Q_8> d_quant_input;
  std::unique_ptr<aff3ct::module::Quantizer_pow2_fast<float, Q_8>> d_quant;

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