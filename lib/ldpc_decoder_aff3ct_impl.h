/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_LDPC_DECODER_AFF3CT_IMPL_H
#define INCLUDED_FEC_DEV_LDPC_DECODER_AFF3CT_IMPL_H

#include <gnuradio/fec_dev/ldpc_decoder_aff3ct.h>
#include <map>
#include <string>

#include "Tools/types.h"
// #include "aff3ct_common_impl.h"
//#include "Module/Encoder/LDPC/Encoder_ldpc.hpp"

namespace gr {
namespace fec_dev {
    
class FEC_API ldpc_decoder_aff3ct_impl : public ldpc_decoder_aff3ct
{
private:
  unsigned int d_K;
  int d_N;
  //std::unique_ptr<aff3ct::module::Encoder_RA<B_8>> d_decoder;

public:
  ldpc_decoder_aff3ct_impl(int K = 0,
                           int N = 0,
                           std::string dec_h_parity_matrix_path="",
                           LDPC::decoder_t dec_type = LDPC::BIT_FLIPPING,
                           LDPC::dec_impl_t dec_impl = LDPC::STD,
                           SIMD::simd_strat_t simd_impl = SIMD::SEQ,
                           LDPC::dec_H_reorder_t dec_h_reorder = LDPC::NONE,
                           LDPC::dec_min_AMS_t dec_min_AMS = LDPC::MINL,
                           float dec_norm_NMS = 1.0,
                           float dec_offset_OMS = 1.0,
                           float dec_mwbf_factor = 0.0,
                           int dec_synd_depth = 1,
                           std::vector<float> ppbf_prob={},
                           bool dec_no_synd = false
                           );
  ~ldpc_decoder_aff3ct_impl() override;

  bool set_frame_size(unsigned int frame_size) override;
  double rate() override;
  int get_output_size() override;
  int get_input_size() override;
  void generic_work(const void* inbuffer, void* outbuffer) override;
};

} // namespace fec_dev
} // namespace gr

#endif /* INCLUDED_FEC_DEV_LDPC_DECODER_AFF3CT_IMPL_H */