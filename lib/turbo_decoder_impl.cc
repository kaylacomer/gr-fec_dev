/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "turbo_decoder_impl.h"

namespace gr {
namespace fec_dev {

fec::generic_decoder::sptr turbo_decoder::make(int frame_bits,
                                      int n_iterations,
                                      Turbo::enc_standard_t standard,
                                      bool buffered,
                                      std::vector<int> polys,
                                      int trellis_size,
                                      uint8_t quant_fixed_point_pos,
                                      uint8_t quant_saturation_pos,
                                      bool set_sat_pos,
                                      Quantizer::quantizer_impl_t quant_impl,
                                      Decoder::decoder_impl_t dec_impl,
                                      BCJR::bcjr_impl_t bcjr_impl,
                                      SIMD::simd_strat_t simd_strat,
                                      SIMD::simd_interintra_impl_t simd_interintra_impl,
                                      Interleaver::interleaver_t interleaver,
                                      Interleaver::itl_read_order_t read_order,
                                      int itl_n_cols)
{
    return fec::generic_decoder::sptr(std::make_shared<turbo_decoder_impl>(
        frame_bits, n_iterations, standard, buffered, polys, trellis_size, quant_fixed_point_pos, quant_saturation_pos, set_sat_pos, quant_impl,
        dec_impl, bcjr_impl, simd_strat, simd_interintra_impl, interleaver, read_order, itl_n_cols));
}

turbo_decoder_impl::turbo_decoder_impl(int frame_bits,
                                      int n_iterations,
                                      Turbo::enc_standard_t standard,
                                      bool buffered,
                                      std::vector<int> polys,
                                      int trellis_size,
                                      uint8_t quant_fixed_point_pos,
                                      uint8_t quant_saturation_pos,
                                      bool set_sat_pos,
                                      Quantizer::quantizer_impl_t quant_impl,
                                      Decoder::decoder_impl_t dec_impl,
                                      BCJR::bcjr_impl_t bcjr_impl,
                                      SIMD::simd_strat_t simd_strat,
                                      SIMD::simd_interintra_impl_t simd_interintra_impl,
                                      Interleaver::interleaver_t interleaver,
                                      Interleaver::itl_read_order_t read_order,
                                      int itl_n_cols)
    : generic_decoder("turbo_decoder"),
      d_K(frame_bits),
      d_trellis_size(trellis_size)
{
    if (standard == Turbo::LTE) {
        d_trellis_size = 8;
        polys = {013,015};
        d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_LTE<>>(d_K);
    }
    else if (standard == Turbo::CCSDS) {
        std::vector<int> CCSDS_sizes = {1784, 3568, 7136, 8920};
        if (!count(CCSDS_sizes.begin(), CCSDS_sizes.end(), d_K)) {
            throw std::runtime_error("CCSDS supported frame sizes are 1784, 3568, 7136, 8920");
        }

        d_trellis_size = 16;
        polys = {023,033};
        d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_CCSDS<>>(d_K);
    }
    else if (interleaver == Interleaver::COL_ROW || interleaver == Interleaver::ROW_COL) {
        std::string order;
        switch (read_order) {
            case Interleaver::TOP_LEFT:
                order = "TOP_LEFT";
                break;
            case Interleaver::TOP_RIGHT:
                order = "TOP_RIGHT";
                break;
            case Interleaver::BOTTOM_LEFT:
                order = "BOTTOM_LEFT";
                break;
            case Interleaver::BOTTOM_RIGHT:
                order = "BOTTOM_RIGHT";
                break;
            default:
                throw std::runtime_error("Need to specify interleaver read order");
        }
        if (interleaver == Interleaver::COL_ROW) d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_column_row<>>(d_K, itl_n_cols, order);
        else d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_row_column<>>(d_K, itl_n_cols, order);
    }
    else {
        switch(interleaver) {
            case Interleaver::NO:
                d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_NO<>>(d_K);
                break;
            case Interleaver::RAND_COL:
                d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_random_column<>>(d_K, itl_n_cols);
                break;
            case Interleaver::GOLDEN:
                d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_golden<>>(d_K);
                break;
            case Interleaver::DVB_RCS1:
                d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_ARP_DVB_RCS1<>>(d_K);
                break;
            case Interleaver::DVB_RCS2:
                d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_ARP_DVB_RCS2<>>(d_K);
                break;
            case Interleaver::RANDOM:
            default:
                d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_random<>>(d_K);
                break;
        }
    }

    d_N = 3 * d_K + 4 * int(std::log2(d_trellis_size));
    int N_rsc = 2 * (d_K + std::log2(d_trellis_size));

    auto enco_n = aff3ct::module::Encoder_RSC_generic_sys<B_8>(d_K, N_rsc, buffered, polys);
    auto enco_i = enco_n;
    
    d_pi = std::make_unique<aff3ct::module::Interleaver<Q_8>>(*d_interleaver_core);

    auto trellis_n = enco_n.get_trellis();
    auto trellis_i = trellis_n;
    
    // aff3ct::module::Decoder_RSC_BCJR<B_8, Q_8> dec_n;
    // aff3ct::module::Decoder_RSC_BCJR<B_8, Q_8> dec_i;

    
    auto dec_n = aff3ct::module::Decoder_RSC_BCJR_seq_generic_std<B_8, Q_8>(d_K, trellis_n, buffered);
    auto dec_i = aff3ct::module::Decoder_RSC_BCJR_seq_generic_std<B_8, Q_8>(d_K, trellis_i, buffered);
    if (simd_strat != SIMD::SEQ || bcjr_impl != BCJR::GENERIC) {
        throw std::runtime_error("simd_strat STD, bcjr_impl GENERIC only supported combination at this time");
    }
    // if (simd_strat == SIMD::SEQ) {
    //     if (bcjr_impl == BCJR::GENERIC) { // default
            // dec_n = aff3ct::module::Decoder_RSC_BCJR_seq_generic_std<B_8, Q_8>(d_K, trellis_n, buffered);
            // dec_i = aff3ct::module::Decoder_RSC_BCJR_seq_generic_std<B_8, Q_8>(d_K, trellis_i, buffered);
    //     }
    //     else if (bcjr_impl == BCJR::SCAN) {
    //         dec_n = std::make_unique<aff3ct::module::Decoder_RSC_BCJR_seq_scan<B_8, Q_8>>(d_K, trellis_n, buffered);
    //         dec_i = std::make_unique<aff3ct::module::Decoder_RSC_BCJR_seq_scan<B_8, Q_8>>(d_K, trellis_i, buffered);
    //     }
    //     else if (bcjr_impl == BCJR::STD) {
    //         dec_n = std::make_unique<aff3ct::module::Decoder_RSC_BCJR_seq_std<B_8, Q_8>>(d_K, trellis_n, buffered);
    //         dec_i = std::make_unique<aff3ct::module::Decoder_RSC_BCJR_seq_std<B_8, Q_8>>(d_K, trellis_i, buffered);
    //     }
    //     else if (bcjr_impl == BCJR::FAST) {
    //         dec_n = std::make_unique<aff3ct::module::Decoder_RSC_BCJR_seq_fast<B_8, Q_8>>(d_K, trellis_n, buffered);
    //         dec_i = std::make_unique<aff3ct::module::Decoder_RSC_BCJR_seq_fast<B_8, Q_8>>(d_K, trellis_i, buffered);
    //     }
    //     else if (bcjr_impl == BCJR::VERY_FAST) {
    //         dec_n = std::make_unique<aff3ct::module::Decoder_RSC_BCJR_seq_very_fast<B_8, Q_8>>(d_K, trellis_n, buffered);
    //         dec_i = std::make_unique<aff3ct::module::Decoder_RSC_BCJR_seq_very_fast<B_8, Q_8>>(d_K, trellis_i, buffered);
    //     }
    //     else {
    //         throw std::runtime_error("BCJR implementations for sequential SIMD include GENERIC (STD), STD, FAST, VERY FAST, and SCAN");
    //     }
    // }
    // else if (simd_strat == SIMD::INTER) {
    //     if (bcjr_impl == BCJR::STD) {
    //         dec_n = std::make_unique<aff3ct::module::Decoder_RSC_BCJR_inter_std<B_8, Q_8>>(d_K, trellis_n, buffered);
    //         dec_i = std::make_unique<aff3ct::module::Decoder_RSC_BCJR_inter_std<B_8, Q_8>>(d_K, trellis_i, buffered);
    //     }
    //     else if (bcjr_impl == BCJR::FAST) {
    //         dec_n = std::make_unique<aff3ct::module::Decoder_RSC_BCJR_inter_fast<B_8, Q_8>>(d_K, trellis_n, buffered);
    //         dec_i = std::make_unique<aff3ct::module::Decoder_RSC_BCJR_inter_fast<B_8, Q_8>>(d_K, trellis_i, buffered);
    //     }
    //     else if (bcjr_impl == BCJR::VERY_FAST) {
    //         dec_n = std::make_unique<aff3ct::module::Decoder_RSC_BCJR_inter_very_fast<B_8, Q_8>>(d_K, trellis_n, buffered);
    //         dec_i = std::make_unique<aff3ct::module::Decoder_RSC_BCJR_inter_very_fast<B_8, Q_8>>(d_K, trellis_i, buffered);
    //     }
    //     else {
    //         throw std::runtime_error("BCJR implementations for Inter SIMD include STD, FAST, and VERY FAST");
    //     }
    // }
    // else if (simd_strat == SIMD::INTRA) {
    //     if (bcjr_impl == BCJR::STD) {
    //         dec_n = std::make_unique<aff3ct::module::Decoder_RSC_BCJR_intra_std<B_8, Q_8>>(d_K, trellis_n, buffered);
    //         dec_i = std::make_unique<aff3ct::module::Decoder_RSC_BCJR_intra_std<B_8, Q_8>>(d_K, trellis_i, buffered);
    //     }
    //     else if (bcjr_impl == BCJR::FAST) {
    //         dec_n = std::make_unique<aff3ct::module::Decoder_RSC_BCJR_intra_fast<B_8, Q_8>>(d_K, trellis_n, buffered);
    //         dec_i = std::make_unique<aff3ct::module::Decoder_RSC_BCJR_intra_fast<B_8, Q_8>>(d_K, trellis_i, buffered);
    //     }
    //     else {
    //         throw std::runtime_error("BCJR implementations for Intra SIMD include STD and FAST");
    //     }
    // }
    // else if (simd_strat == SIMD::INTER_INTRA) {
    //     if (bcjr_impl == BCJR::FAST) {
    //         if (simd_interintra_impl == SIMD::simd_interintra_impl_t::X2_AVX) {
    //             dec_n = std::make_unique<aff3ct::module::Decoder_RSC_BCJR_inter_intra_fast_x2_AVX<B_8, Q_8>>(d_K, trellis_n, buffered);
    //             dec_i = std::make_unique<aff3ct::module::Decoder_RSC_BCJR_inter_intra_fast_x2_AVX<B_8, Q_8>>(d_K, trellis_i, buffered);
    //         }
    //         else if (simd_interintra_impl == SIMD::simd_interintra_impl_t::X2_SSE) {
    //             dec_n = std::make_unique<aff3ct::module::Decoder_RSC_BCJR_inter_intra_fast_x2_SSE<B_8, Q_8>>(d_K, trellis_n, buffered);
    //             dec_i = std::make_unique<aff3ct::module::Decoder_RSC_BCJR_inter_intra_fast_x2_SSE<B_8, Q_8>>(d_K, trellis_i, buffered);
    //         }
    //         else if (simd_interintra_impl == SIMD::simd_interintra_impl_t::X4_AVX) {
    //             dec_n = std::make_unique<aff3ct::module::Decoder_RSC_BCJR_inter_intra_fast_x4_AVX<B_8, Q_8>>(d_K, trellis_n, buffered);
    //             dec_i = std::make_unique<aff3ct::module::Decoder_RSC_BCJR_inter_intra_fast_x4_AVX<B_8, Q_8>>(d_K, trellis_i, buffered);
    //         }
    //         else {
    //             throw std::runtime_error("SIMD INTER INTRA implementation must be X2_AVX, X4_AVX, or X2_SSE");
    //         }
    //     }
    //     else {
    //         throw std::runtime_error("BCJR implementation for INTER_INTRA SIMD must be FAST");
    //     }
    // }
    // else {
    //     throw std::runtime_error("SIMD strategies (simd_strat) for BCJR include SEQ, INTER, INTRA, and INTER_INTRA");
    // }

    if (dec_impl == Decoder::STD) {
        d_decoder = std::make_unique<aff3ct::module::Decoder_turbo_std<B_8, Q_8>>(d_K, d_N, n_iterations, dec_n, dec_i, *d_pi, buffered);
    }
    else if (dec_impl == Decoder::FAST) {
        d_decoder = std::make_unique<aff3ct::module::Decoder_turbo_fast<B_8, Q_8>>(d_K, d_N, n_iterations, dec_n, dec_i, *d_pi, buffered);
    }
    else {
        throw std::runtime_error("Turbo decoder has standard and fast implementations. Set dec_impl to STD or FAST");
    }

    if (quant_impl == Quantizer::STD) {
        if (set_sat_pos) {
            d_quant = std::make_unique<aff3ct::module::Quantizer_pow2<float, Q_8>>(d_N, quant_fixed_point_pos, quant_saturation_pos);
        }
        else {
            d_quant = std::make_unique<aff3ct::module::Quantizer_pow2<float, Q_8>>(d_N, quant_fixed_point_pos);
        }
    }
    else if (quant_impl == Quantizer::FAST) {
        if (set_sat_pos) {
            d_quant = std::make_unique<aff3ct::module::Quantizer_pow2_fast<float, Q_8>>(d_N, quant_fixed_point_pos, quant_saturation_pos);
        }
        else {
            d_quant = std::make_unique<aff3ct::module::Quantizer_pow2_fast<float, Q_8>>(d_N, quant_fixed_point_pos);
        }
    }
    else {
        d_quant = std::make_unique<aff3ct::module::Quantizer_NO<float, Q_8>>(d_N);
    }
    d_quant_input = std::vector<Q_8>(d_N);
    d_tmp_input = std::vector<float>(d_N);
}

turbo_decoder_impl::~turbo_decoder_impl() {}

int turbo_decoder_impl::get_output_size() { return d_K; }

int turbo_decoder_impl::get_input_size() { return d_N; }

int turbo_decoder_impl::get_input_item_size() { return sizeof(float); }

const char* turbo_decoder_impl::get_input_conversion() { return "none"; }

bool turbo_decoder_impl::set_frame_size(unsigned int frame_bits)
{
    return true;
}

double turbo_decoder_impl::rate() { return static_cast<float>(d_K) / d_N; }

void turbo_decoder_impl::generic_work(const void* inbuffer, void* outbuffer)
{
    const float* in = (const float*)inbuffer;
    B_8* out = (B_8*)outbuffer;

    volk_32f_s32f_multiply_32f(d_tmp_input.data(), in, -1.0f, d_N);
    d_quant->process(d_tmp_input.data(), d_quant_input.data(), -1);
    
    int status = d_decoder->decode_siho(d_quant_input.data(), out, -1);
    if (status == 1) {
        std::cout << "Decoding FAILURE" << std::endl;
    }
}

} // namespace fec_dev
} /* namespace gr */
