/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/io_signature.h>
#include "rsc_decoder_impl.h"
#include <volk/volk.h>

namespace gr {
namespace fec_dev {

fec::generic_decoder::sptr rsc_decoder::make(int K, 
                  std::vector<int> polys, 
                  int trellis_size,
                  bool buffered,
                  RSC::rsc_decoder_impl_t dec_type,
                  uint8_t quant_fixed_point_pos,
                  uint8_t quant_saturation_pos,
                  Quantizer::quantizer_impl_t quant_impl,
                  BCJR::bcjr_impl_t bcjr_impl,
                  SIMD::simd_strat_t simd_strat,
                  SIMD::simd_interintra_impl_t simd_interintra_impl,
                  std::string viterbi_list_poly_key,
                  bool viterbi_siho_is_closed)
{
    return fec::generic_decoder::sptr(std::make_shared<rsc_decoder_impl>(K, polys, trellis_size, buffered, dec_type,
        quant_fixed_point_pos, quant_saturation_pos, quant_impl, bcjr_impl, simd_strat, simd_interintra_impl,
        viterbi_list_poly_key, viterbi_siho_is_closed));
}
    rsc_decoder_impl::rsc_decoder_impl(int K, 
                  std::vector<int> polys, 
                  int trellis_size,
                  bool buffered,
                  RSC::rsc_decoder_impl_t dec_type,
                  uint8_t quant_fixed_point_pos,
                  uint8_t quant_saturation_pos,
                  Quantizer::quantizer_impl_t quant_impl,
                  BCJR::bcjr_impl_t bcjr_impl,
                  SIMD::simd_strat_t simd_strat,
                  SIMD::simd_interintra_impl_t simd_interintra_impl,
                  std::string viterbi_list_poly_key,
                  bool viterbi_siho_is_closed)
        : generic_decoder("rsc_decoder"),
        d_K(K),
        d_trellis_size(trellis_size)
    {
        set_frame_size(K);

        if (quant_impl == Quantizer::STD) {
            d_quant = std::make_unique<aff3ct::module::Quantizer_pow2<float, Q_8>>(d_N, quant_fixed_point_pos, quant_saturation_pos);
        }
        else if (quant_impl == Quantizer::FAST) {
            d_quant = std::make_unique<aff3ct::module::Quantizer_pow2_fast<float, Q_8>>(d_N, quant_fixed_point_pos, quant_saturation_pos);
        }
        else {
            d_quant = std::make_unique<aff3ct::module::Quantizer_NO<float, Q_8>>(d_N);
        }
        d_quant_input = std::vector<Q_8>(d_N);
        d_tmp_input = std::vector<float>(d_N);

        auto enco_n = aff3ct::module::Encoder_RSC_generic_sys<B_8>(d_K, d_N, buffered, polys);
        auto trellis_n = enco_n.get_trellis();

        if (dec_type == RSC::Viterbi_SIHO) {
            d_decoder = std::make_unique<aff3ct::module::Decoder_Viterbi_SIHO<B_8, Q_8>>(d_K, trellis_n, viterbi_siho_is_closed);
        }
        else if (dec_type == RSC::Viterbi_list) {
            auto crc = aff3ct::module::CRC_polynomial<B_8>(d_K, viterbi_list_poly_key);
            int size = crc.get_size(viterbi_list_poly_key);
            d_decoder = std::make_unique<aff3ct::module::Decoder_Viterbi_list_parallel<B_8, Q_8>>(d_K, d_N, size, crc, trellis_n, viterbi_siho_is_closed);
        }

        // else if (dec_type == RSC::BCJR) {
        //     if (simd_strat == SIMD::SEQ) {
        //         if (bcjr_impl == BCJR::GENERIC) { // default
        //             d_decoder = aff3ct::module::Decoder_RSC_BCJR_seq_generic_std<B_8, Q_8>(d_K, trellis_n, buffered);
        //         }
        //         else if (bcjr_impl == BCJR::SCAN) {
        //             d_decoder = std::make_unique<aff3ct::module::Decoder_RSC_BCJR_seq_scan<B_8, Q_8>>(d_K, trellis_n, buffered);
        //         }
        //         else if (bcjr_impl == BCJR::STD) {
        //             d_decoder = std::make_unique<aff3ct::module::Decoder_RSC_BCJR_seq_std<B_8, Q_8>>(d_K, trellis_n, buffered);
        //         }
        //         else if (bcjr_impl == BCJR::FAST) {
        //             d_decoder = std::make_unique<aff3ct::module::Decoder_RSC_BCJR_seq_fast<B_8, Q_8>>(d_K, trellis_n, buffered);
        //         }
        //         else if (bcjr_impl == BCJR::VERY_FAST) {
        //             d_decoder = std::make_unique<aff3ct::module::Decoder_RSC_BCJR_seq_very_fast<B_8, Q_8>>(d_K, trellis_n, buffered);
        //         }
        //         else {
        //             throw std::runtime_error("BCJR implementations for sequential SIMD include GENERIC (STD), STD, FAST, VERY FAST, and SCAN");
        //         }
        //     }
        //     else if (simd_strat == SIMD::INTER) {
        //         if (bcjr_impl == BCJR::STD) {
        //             d_decoder = std::make_unique<aff3ct::module::Decoder_RSC_BCJR_inter_std<B_8, Q_8>>(d_K, trellis_n, buffered);
        //         }
        //         else if (bcjr_impl == BCJR::FAST) {
        //             d_decoder = std::make_unique<aff3ct::module::Decoder_RSC_BCJR_inter_fast<B_8, Q_8>>(d_K, trellis_n, buffered);
        //         }
        //         else if (bcjr_impl == BCJR::VERY_FAST) {
        //             d_decoder = std::make_unique<aff3ct::module::Decoder_RSC_BCJR_inter_very_fast<B_8, Q_8>>(d_K, trellis_n, buffered);
        //         }
        //         else {
        //             throw std::runtime_error("BCJR implementations for Inter SIMD include STD, FAST, and VERY FAST");
        //         }
        //     }
        //     else if (simd_strat == SIMD::INTRA) {
        //         if (bcjr_impl == BCJR::STD) {
        //             d_decoder = std::make_unique<aff3ct::module::Decoder_RSC_BCJR_intra_std<B_8, Q_8>>(d_K, trellis_n, buffered);
        //         }
        //         else if (bcjr_impl == BCJR::FAST) {
        //             d_decoder = std::make_unique<aff3ct::module::Decoder_RSC_BCJR_intra_fast<B_8, Q_8>>(d_K, trellis_n, buffered);
        //         }
        //         else {
        //             throw std::runtime_error("BCJR implementations for Intra SIMD include STD and FAST");
        //         }
        //     }
        //     else if (simd_strat == SIMD::INTER_INTRA) {
        //         if (bcjr_impl == BCJR::FAST) {
        //             if (simd_interintra_impl == SIMD::simd_interintra_impl_t::X2_AVX) {
        //                 d_decoder = std::make_unique<aff3ct::module::Decoder_RSC_BCJR_inter_intra_fast_x2_AVX<B_8, Q_8>>(d_K, trellis_n, buffered);
        //             }
        //             else if (simd_interintra_impl == SIMD::simd_interintra_impl_t::X2_SSE) {
        //                 d_decoder = std::make_unique<aff3ct::module::Decoder_RSC_BCJR_inter_intra_fast_x2_SSE<B_8, Q_8>>(d_K, trellis_n, buffered);
        //             }
        //             else if (simd_interintra_impl == SIMD::simd_interintra_impl_t::X4_AVX) {
        //                 d_decoder = std::make_unique<aff3ct::module::Decoder_RSC_BCJR_inter_intra_fast_x4_AVX<B_8, Q_8>>(d_K, trellis_n, buffered);
        //             }
        //             else {
        //                 throw std::runtime_error("SIMD INTER INTRA implementation must be X2_AVX, X4_AVX, or X2_SSE");
        //             }
        //         }
        //         else {
        //             throw std::runtime_error("BCJR implementation for INTER_INTRA SIMD must be FAST");
        //         }
        //     }
            else {
                throw std::runtime_error("SIMD strategies (simd_strat) for BCJR include SEQ, INTER, INTRA, and INTER_INTRA");
            }
        // }
}

rsc_decoder_impl::~rsc_decoder_impl()
{
}

int rsc_decoder_impl::get_output_size() { return d_K; }
int rsc_decoder_impl::get_input_size() { return d_N; }

bool rsc_decoder_impl::set_frame_size(unsigned int K)
{
    d_N = 2 * (d_K + std::log2(d_trellis_size));
    return true;
}

double rsc_decoder_impl::rate() { return static_cast<float>(d_K) / d_N; } // decoder rate

void rsc_decoder_impl::generic_work(const void* inbuffer, void* outbuffer)
{
    const float* in = (const float*)inbuffer;
    B_8* out = (B_8*)outbuffer;

    volk_32f_s32f_multiply_32f(d_tmp_input.data(), in, -1.0f, d_N);
    d_quant->process(d_tmp_input.data(), d_quant_input.data(), -1);
    
    // std::vector<B_8> temp_output(d_K);
    int status = d_decoder->decode_siho(d_quant_input.data(), out, -1);
    if (status == 1) {
        std::cout << "Decoding FAILURE" << std::endl;
    }

    // std::memcpy(out, temp_output.data(), d_K * sizeof(B_8));
}

} /* namespace fec_dev */
} /* namespace gr */