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
#include "Module/Decoder/RSC/Viterbi_list/Decoder_Viterbi_list_parallel.hpp"
#include "Module/CRC/Polynomial/CRC_polynomial.hpp"
// #include "Module/CRC/Polynomial/CRC_polynomial.hxx"
#include "Module/Decoder/RSC/BCJR/Seq/Decoder_RSC_BCJR_seq_fast.hpp"
#include "Module/Decoder/RSC/BCJR/Seq_generic/Decoder_RSC_BCJR_seq_generic_std.hpp"
#include "Module/Decoder/RSC/BCJR/Seq/Decoder_RSC_BCJR_seq_scan.hpp"
#include "Module/Decoder/RSC/BCJR/Seq/Decoder_RSC_BCJR_seq_std.hpp"
#include "Module/Decoder/RSC/BCJR/Seq/Decoder_RSC_BCJR_seq_fast.hpp"
#include "Module/Decoder/RSC/BCJR/Seq/Decoder_RSC_BCJR_seq_very_fast.hpp"
#include "Module/Decoder/RSC/BCJR/Inter/Decoder_RSC_BCJR_inter_fast.hpp"
#include "Module/Decoder/RSC/BCJR/Inter/Decoder_RSC_BCJR_inter_std.hpp"
#include "Module/Decoder/RSC/BCJR/Inter/Decoder_RSC_BCJR_inter_very_fast.hpp"
#include "Module/Decoder/RSC/BCJR/Intra/Decoder_RSC_BCJR_intra_fast.hpp"
#include "Module/Decoder/RSC/BCJR/Intra/Decoder_RSC_BCJR_intra_std.hpp"
#include "Module/Decoder/RSC/BCJR/Inter_intra/Decoder_RSC_BCJR_inter_intra_fast_x2_AVX.hpp"
#include "Module/Decoder/RSC/BCJR/Inter_intra/Decoder_RSC_BCJR_inter_intra_fast_x2_SSE.hpp"
#include "Module/Decoder/RSC/BCJR/Inter_intra/Decoder_RSC_BCJR_inter_intra_fast_x4_AVX.hpp"

#include "aff3ct_quantizer_headers.h"

namespace gr {
namespace fec_dev {
    
class FEC_API rsc_decoder_impl : public rsc_decoder
{
private:
  unsigned int d_K;
  int d_N;
  int d_trellis_size;
  std::unique_ptr<aff3ct::module::Decoder_SIHO<B_8, Q_8>> d_decoder;
  std::vector<float> d_tmp_input;
  std::vector<Q_8> d_quant_input;
  std::unique_ptr<aff3ct::module::Quantizer<float, Q_8>> d_quant;

public:
  rsc_decoder_impl(int K, 
                  std::vector<int> polys={013,015}, 
                  int trellis_size = 8,
                  bool buffered=true,
                  RSC::rsc_decoder_impl_t dec_type = RSC::BCJR,
                  uint8_t quant_fixed_point_pos = 1,
                  uint8_t quant_saturation_pos = 6,
                  Quantizer::quantizer_impl_t quant_impl=Quantizer::STD,
                  BCJR::bcjr_impl_t bcjr_impl=BCJR::GENERIC,
                  SIMD::simd_strat_t simd_strat=SIMD::SEQ,
                  SIMD::simd_interintra_impl_t simd_interintra_impl=SIMD::NA,
                  std::string viterbi_list_poly_key="none",
                  bool viterbi_siho_is_closed=true);
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