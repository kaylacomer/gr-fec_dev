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
                                      Quantizer::quantizer_impl_t quant_impl,
                                      Turbo::subenc_implem_t subenc_impl,
                                      int n_ff,
                                      Decoder::decoder_impl_t dec_impl,
                                      BCJR::bcjr_impl_t bcjr_impl,
                                      SIMD::simd_strat_t simd_strat,
                                      SIMD::simd_interintra_impl_t simd_interintra_impl,
                                      Interleaver::itl_read_order_t read_order,
                                      int itl_n_cols)
{
    return fec::generic_decoder::sptr(new turbo_decoder_impl(
        frame_bits, n_iterations, standard, buffered, polys, trellis_size, quant_impl,
        subenc_impl, n_ff, dec_impl, bcjr_impl, simd_strat, simd_interintra_impl, read_order, itl_n_cols));
}

turbo_decoder_impl::turbo_decoder_impl(int frame_bits,
                                      int n_iterations,
                                      Turbo::enc_standard_t standard,
                                      bool buffered,
                                      std::vector<int> polys,
                                      int trellis_size,
                                      Quantizer::quantizer_impl_t quant_impl,
                                      Turbo::subenc_implem_t subenc_impl,
                                      int n_ff,
                                      Decoder::decoder_impl_t dec_impl,
                                      BCJR::bcjr_impl_t bcjr_impl,
                                      SIMD::simd_strat_t simd_strat,
                                      SIMD::simd_interintra_impl_t simd_interintra_impl,
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
    // else if (standard == NO) {
    //     d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_NO<>>(d_K);
    // }
    // // else if (standard == COL_ROW) {
    // //     const int n_cols = 1;
    // //     itl_read_order_t read_order = TOP_LEFT;
    // //     d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_column_row<>>(d_K, n_cols, read_order);
    // // }
    // // else if (standard == ROW_COL) {
    // //     const int n_cols = 1;
    // //     itl_read_order_t read_order = TOP_LEFT;
    // //     d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_row_column<>>(d_K, n_cols, read_order);
    // // }
    // else if (standard == RAND_COL) {
    //     int n_cols = 1;
    //     d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_random_column<>>(d_K, n_cols);
    // }
    // else if (standard == GOLDEN) {
    //     d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_golden<>>(d_K);
    // }
    // else if (standard == DVB_RCS1) {
    //     d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_ARP_DVB_RCS1<>>(d_K);
    // }
    // else if (standard == DVB_RCS2) {
    //     d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_ARP_DVB_RCS2<>>(d_K);
    // }
    else {
        d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_random<>>(d_K);
    }

    d_N = 3 * d_K + 4 * std::log2(d_trellis_size);

    int N_rsc = 2 * (d_K + std::log2(d_trellis_size));
    auto enco_n = aff3ct::module::Encoder_RSC_generic_sys<B_8>(d_K, N_rsc, buffered, polys);
    auto enco_i = enco_n;

    d_pi = std::make_unique<aff3ct::module::Interleaver<Q_8>>(*d_interleaver_core);

    auto trellis_n = enco_n.get_trellis();
    auto trellis_i = trellis_n;

    auto dec_n = aff3ct::module::Decoder_RSC_BCJR_seq_generic_std<B_8, Q_8>(d_K, trellis_n, buffered);
    auto dec_i = aff3ct::module::Decoder_RSC_BCJR_seq_generic_std<B_8, Q_8>(d_K, trellis_i, buffered);

    d_decoder = std::make_unique<aff3ct::module::Decoder_turbo_fast<B_8, Q_8>>(d_K, d_N, n_iterations, dec_n, dec_i, *d_pi, buffered);

    d_quant = std::make_unique<aff3ct::module::Quantizer_pow2_fast<float, Q_8>>(d_N, 2);
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
