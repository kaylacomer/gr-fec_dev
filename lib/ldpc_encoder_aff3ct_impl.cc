/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/io_signature.h>
#include "ldpc_encoder_aff3ct_impl.h"

namespace gr {
namespace fec_dev {

fec::generic_encoder::sptr ldpc_encoder_aff3ct::make(int K,
                           int N,
                           LDPC::encoder_t enc_type,
                           std::string enc_gen_matrix_path,
                           LDPC::enc_gen_matrix_method_t enc_gen_matrix_method,
                           std::string enc_gen_matrix_save_path)
{
    return fec::generic_encoder::sptr(std::make_shared<ldpc_encoder_aff3ct_impl>(K, N, enc_type, 
        enc_gen_matrix_path, enc_gen_matrix_method, enc_gen_matrix_save_path));
}
    ldpc_encoder_aff3ct_impl::ldpc_encoder_aff3ct_impl(int K,
                           int N,
                           LDPC::encoder_t enc_type,
                           std::string enc_gen_matrix_path,
                           LDPC::enc_gen_matrix_method_t enc_gen_matrix_method,
                           std::string enc_gen_matrix_save_path)
        : generic_encoder("ldpc_encoder_aff3ct"),
        d_K(K)
    {
        set_frame_size(K);

        throw std::runtime_error("AFF3CT-based LDPC encoder has not been implemented");
        //d_encoder = std::make_unique<aff3ct::module::Encoder_RA<B_8>>(d_K, d_N, *d_interleaver);
}

ldpc_encoder_aff3ct_impl::~ldpc_encoder_aff3ct_impl()
{
}

int ldpc_encoder_aff3ct_impl::get_output_size() { return d_N; }
int ldpc_encoder_aff3ct_impl::get_input_size() { return d_K; }

bool ldpc_encoder_aff3ct_impl::set_frame_size(unsigned int K)
{
    return true;
}

double ldpc_encoder_aff3ct_impl::rate() { return static_cast<float>(d_N) / d_K; } // block output:input rate, inverse of codec rate

void ldpc_encoder_aff3ct_impl::generic_work(const void* inbuffer, void* outbuffer)
{
    const B_8* in = (const B_8*)inbuffer;
    B_8* out = (B_8*)outbuffer;

    //d_encoder->encode(in, out);
}

} /* namespace fec_dev */
} /* namespace gr */