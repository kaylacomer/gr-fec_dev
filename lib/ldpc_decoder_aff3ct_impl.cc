/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/io_signature.h>
#include "ldpc_decoder_aff3ct_impl.h"

namespace gr {
namespace fec_dev {

fec::generic_decoder::sptr ldpc_decoder_aff3ct::make(int K,
                           int N,
                           std::string dec_h_parity_matrix_path,
                           LDPC::decoder_t dec_type,
                           LDPC::dec_impl_t dec_impl,
                           SIMD::simd_strat_t simd_impl,
                           LDPC::dec_H_reorder_t dec_h_reorder,
                           LDPC::dec_min_AMS_t dec_min_AMS,
                           float dec_norm_NMS,
                           float dec_offset_OMS,
                           float dec_mwbf_factor,
                           int dec_synd_depth,
                           std::vector<float> ppbf_prob,
                           bool dec_no_synd)
{
    return fec::generic_decoder::sptr(std::make_shared<ldpc_decoder_aff3ct_impl>(K, N, dec_h_parity_matrix_path,
        dec_type, dec_impl, simd_impl, dec_h_reorder, dec_min_AMS, dec_norm_NMS, dec_offset_OMS,
        dec_mwbf_factor, dec_synd_depth, ppbf_prob, dec_no_synd));
}
    ldpc_decoder_aff3ct_impl::ldpc_decoder_aff3ct_impl(int K,
                           int N,
                           std::string dec_h_parity_matrix_path,
                           LDPC::decoder_t dec_type,
                           LDPC::dec_impl_t dec_impl,
                           SIMD::simd_strat_t simd_impl,
                           LDPC::dec_H_reorder_t dec_h_reorder,
                           LDPC::dec_min_AMS_t dec_min_AMS,
                           float dec_norm_NMS,
                           float dec_offset_OMS,
                           float dec_mwbf_factor,
                           int dec_synd_depth,
                           std::vector<float> ppbf_prob,
                           bool dec_no_synd)
        : generic_decoder("ldpc_decoder_aff3ct"),
        d_K(K)
    {
        set_frame_size(K);

        throw std::runtime_error("AFF3CT-based LDPC decoder has not been implemented");
        //d_decoder = std::make_unique<aff3ct::module::decoder_RA<B_8>>(d_K, d_N, *d_interleaver);
}

ldpc_decoder_aff3ct_impl::~ldpc_decoder_aff3ct_impl()
{
}

int ldpc_decoder_aff3ct_impl::get_output_size() { return d_K; }
int ldpc_decoder_aff3ct_impl::get_input_size() { return d_N; }

bool ldpc_decoder_aff3ct_impl::set_frame_size(unsigned int K)
{
    return true;
}

double ldpc_decoder_aff3ct_impl::rate() { return static_cast<float>(d_K) / d_N; } // decoder rate

void ldpc_decoder_aff3ct_impl::generic_work(const void* inbuffer, void* outbuffer)
{
    const float* in = (const float*)inbuffer;
    B_8* out = (B_8*)outbuffer;

    // volk_32f_s32f_multiply_32f(d_tmp_input.data(), in, -1.0f, d_N);
    // d_quant->process(d_tmp_input.data(), d_quant_input.data(), -1);
    
    // int status = d_decoder->decode_siho(d_quant_input.data(), out, -1);
    // if (status == 1) {
    //     std::cout << "Decoding FAILURE" << std::endl;
    // }
}

} /* namespace fec_dev */
} /* namespace gr */