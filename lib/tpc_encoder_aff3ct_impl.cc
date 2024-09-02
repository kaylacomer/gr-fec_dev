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
        d_K_sqrt(K_sqrt),
        d_N_sqrt(N_sqrt)
    {
        set_frame_size(K_sqrt);

        d_poly_gen = std::make_unique<aff3ct::tools::BCH_polynomial_generator<B_8>>(N_sqrt, t);
        int rdncy = d_poly_gen->get_n_rdncy();
        d_K_sqrt = N_sqrt - rdncy;

        auto enc_r = aff3ct::module::Encoder_BCH<B_8>(K_sqrt, N_sqrt, *d_poly_gen);
        enc_r.set_n_frames(N_sqrt);
        auto enc_c = enc_r;

        int N = N_sqrt * N_sqrt;
        int itl_n_cols = N_sqrt;

        if (interleaver == Interleaver::COL_ROW || interleaver == Interleaver::ROW_COL) {
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
            if (interleaver == Interleaver::COL_ROW) d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_column_row<>>(N, itl_n_cols, order);
            else d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_row_column<>>(N, itl_n_cols, order);
        }
        else {
            switch(interleaver) {
                case Interleaver::NO:
                    d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_NO<>>(N);
                    break;
                case Interleaver::RAND_COL:
                    d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_random_column<>>(N, itl_n_cols);
                    break;
                case Interleaver::GOLDEN:
                    d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_golden<>>(N);
                    break;
                case Interleaver::DVB_RCS1:
                    d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_ARP_DVB_RCS1<>>(N);
                    break;
                case Interleaver::DVB_RCS2:
                    d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_ARP_DVB_RCS2<>>(N);
                    break;
                case Interleaver::LTE:
                    d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_LTE<>>(N);
                    break;
                case Interleaver::CCSDS:
                    d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_CCSDS<>>(N);
                    break;
                case Interleaver::RANDOM:
                default:
                    d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_random<>>(N);
                    break;
            }
        }

        d_pi = std::make_unique<aff3ct::module::Interleaver<B_8>>(*d_interleaver_core);

        d_encoder = std::make_unique<aff3ct::module::Encoder_turbo_product<B_8>>(enc_r, enc_c, *d_pi);
}

tpc_encoder_aff3ct_impl::~tpc_encoder_aff3ct_impl()
{
}

int tpc_encoder_aff3ct_impl::get_output_size() { return d_N_sqrt * d_N_sqrt; }
int tpc_encoder_aff3ct_impl::get_input_size() { return d_K_sqrt * d_K_sqrt; }

bool tpc_encoder_aff3ct_impl::set_frame_size(unsigned int K)
{
    return true;
}

double tpc_encoder_aff3ct_impl::rate() { return static_cast<float>(d_N_sqrt * d_N_sqrt) / d_K_sqrt * d_K_sqrt; } // block output:input rate, inverse of codec rate

void tpc_encoder_aff3ct_impl::generic_work(const void* inbuffer, void* outbuffer)
{
    const B_8* in = (const B_8*)inbuffer;
    B_8* out = (B_8*)outbuffer;

    d_encoder->encode(in, out);
}

} /* namespace fec_dev */
} /* namespace gr */