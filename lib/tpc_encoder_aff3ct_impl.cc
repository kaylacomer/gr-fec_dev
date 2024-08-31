/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/io_signature.h>
#include "tpc_encoder_aff3ct_impl.h"

namespace gr {
namespace fec_dev {

fec::generic_encoder::sptr tpc_encoder_aff3ct::make(int K_sqrt,
                                                    int N_sqrt,
                                                    int t,
                                                    SIMD::simd_strat_t bch_simd_strat,
                                                    Interleaver::interleaver_t interleaver,
                                                    Interleaver::itl_read_order_t read_order,
                                                    bool parity_extended)
{
    return fec::generic_encoder::sptr(std::make_shared<tpc_encoder_aff3ct_impl>(K_sqrt, N_sqrt, t, bch_simd_strat, interleaver, read_order, parity_extended));
}
    tpc_encoder_aff3ct_impl::tpc_encoder_aff3ct_impl(int K_sqrt,
                                                    int N_sqrt,
                                                    int t,
                                                    SIMD::simd_strat_t bch_simd_strat,
                                                    Interleaver::interleaver_t interleaver,
                                                    Interleaver::itl_read_order_t read_order,
                                                    bool parity_extended)
        : generic_encoder("tpc_encoder_aff3ct"),
        d_K(45),
        d_N(63*63)
    {
        set_frame_size(K_sqrt);

        d_poly_gen = std::make_unique<aff3ct::tools::BCH_polynomial_generator<B_8>>(std::sqrt(d_N), t);
        int rdncy = d_poly_gen->get_n_rdncy();
        d_K = std::sqrt(d_N) - rdncy;
        // auto enc_r = std::make_unique<aff3ct::module::Encoder_BCH<B_8>>(d_K, d_N, *d_poly_gen);
        // auto enc_c = enc_r;

        auto enc_r = aff3ct::module::Encoder_BCH<B_8>(d_K, std::sqrt(d_N), *d_poly_gen);
        enc_r.set_n_frames(std::sqrt(d_N));
        auto enc_c = enc_r;

        // const aff3ct::tools::Interleaver_core_row_column<T>::READ_ORDER read_order
        // const std::string& read_order
        // "TOP_LEFT"
    //     Interleaver_core_row_column(const int size, const int n_cols, const std::string& read_order);
    // Interleaver_core_row_column(const int size,
    //                             const int n_cols,
    //                             const aff3ct::tools::Interleaver_core_row_column<T>::READ_ORDER read_order);
        d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_row_column<>>(d_N, std::sqrt(d_N), "TOP_LEFT");
        // d_interleaver_core->set_n_frames(d_K);
        d_pi = std::make_unique<aff3ct::module::Interleaver<B_8>>(*d_interleaver_core);

        //  Encoder_turbo_product(const Encoder<B>& enc_r, const Encoder<B>& enc_c, const Interleaver<B>& pi);

        d_encoder = std::make_unique<aff3ct::module::Encoder_turbo_product<B_8>>(enc_r, enc_c, *d_pi);
}

tpc_encoder_aff3ct_impl::~tpc_encoder_aff3ct_impl()
{
}

int tpc_encoder_aff3ct_impl::get_output_size() { return d_N; }
int tpc_encoder_aff3ct_impl::get_input_size() { return d_K; }

bool tpc_encoder_aff3ct_impl::set_frame_size(unsigned int K)
{
    return true;
}

double tpc_encoder_aff3ct_impl::rate() { return static_cast<float>(d_N) / d_K; } // block output:input rate, inverse of codec rate

void tpc_encoder_aff3ct_impl::generic_work(const void* inbuffer, void* outbuffer)
{
    const B_8* in = (const B_8*)inbuffer;
    B_8* out = (B_8*)outbuffer;

    d_encoder->encode(in, out);
}

} /* namespace fec_dev */
} /* namespace gr */