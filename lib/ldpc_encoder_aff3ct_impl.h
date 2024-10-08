/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_LDPC_ENCODER_AFF3CT_IMPL_H
#define INCLUDED_FEC_DEV_LDPC_ENCODER_AFF3CT_IMPL_H

#include <gnuradio/fec_dev/ldpc_encoder_aff3ct.h>
#include <map>
#include <string>

#include "Tools/types.h"
// #include "aff3ct_common_impl.h"
//#include "Module/Encoder/LDPC/Encoder_ldpc.hpp"

namespace gr {
namespace fec_dev {
    
class FEC_API ldpc_encoder_aff3ct_impl : public ldpc_encoder_aff3ct
{
private:
  unsigned int d_K;
  int d_N;
  //std::unique_ptr<aff3ct::module::Encoder_RA<B_8>> d_encoder;

public:
  ldpc_encoder_aff3ct_impl(int K = 0,
                           int N = 0,
                           LDPC::encoder_t enc_type = LDPC::LDPC,
                           std::string enc_gen_matrix_path="",
                           LDPC::enc_gen_matrix_method_t enc_gen_matrix_method = LDPC::IDENTITY,
                           std::string enc_gen_matrix_save_path=""
                           );
  ~ldpc_encoder_aff3ct_impl() override;

  bool set_frame_size(unsigned int frame_size) override;
  double rate() override;
  int get_output_size() override;
  int get_input_size() override;
  void generic_work(const void* inbuffer, void* outbuffer) override;
};

} // namespace fec_dev
} // namespace gr

#endif /* INCLUDED_FEC_DEV_LDPC_ENCODER_AFF3CT_IMPL_H */