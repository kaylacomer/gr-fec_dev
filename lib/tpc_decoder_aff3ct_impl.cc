/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/io_signature.h>
#include "tpc_decoder_aff3ct_impl.h"
#include <volk/volk.h>

namespace gr {
namespace fec_dev {

fec::generic_decoder::sptr tpc_decoder_aff3ct::make(int K_sqrt,
                                                    int N_sqrt,
                                                    int t,
                                                    SIMD::simd_strat_t bch_simd_strat,
                                                    Interleaver::interleaver_t interleaver,
                                                    Interleaver::itl_read_order_t read_order,
                                                    bool parity_extended,
                                                    uint8_t quant_fixed_point_pos,
                                                    uint8_t quant_saturation_pos,
                                                    Quantizer::quantizer_impl_t quant_impl,
                                                    Decoder::decoder_impl_t bch_dec_impl,
                                                    Decoder::decoder_impl_t chase_pyndiah_impl,
                                                    int n_iterations,
                                                    std::vector<float> alpha,
                                                    std::vector<float> beta,
                                                    std::vector<float> chase_pyndiah_coef,
                                                    int n_least_reliable_pos,
                                                    int n_test_vectors,
                                                    int n_competitors)
{
    return fec::generic_decoder::sptr(std::make_shared<tpc_decoder_aff3ct_impl>(K_sqrt, N_sqrt, t, bch_simd_strat, interleaver, read_order,
                                                    parity_extended, quant_fixed_point_pos, quant_saturation_pos, quant_impl, bch_dec_impl,
                                                    chase_pyndiah_impl, n_iterations, alpha, beta, chase_pyndiah_coef, n_least_reliable_pos,
                                                    n_test_vectors, n_competitors));
}
    tpc_decoder_aff3ct_impl::tpc_decoder_aff3ct_impl(int K_sqrt,
                                                    int N_sqrt,
                                                    int t,
                                                    SIMD::simd_strat_t bch_simd_strat,
                                                    Interleaver::interleaver_t interleaver,
                                                    Interleaver::itl_read_order_t read_order,
                                                    bool parity_extended,
                                                    uint8_t quant_fixed_point_pos,
                                                    uint8_t quant_saturation_pos,
                                                    Quantizer::quantizer_impl_t quant_impl,
                                                    Decoder::decoder_impl_t bch_dec_impl,
                                                    Decoder::decoder_impl_t chase_pyndiah_impl,
                                                    int n_iterations,
                                                    std::vector<float> alpha,
                                                    std::vector<float> beta,
                                                    std::vector<float> chase_pyndiah_coef,
                                                    int n_least_reliable_pos,
                                                    int n_test_vectors,
                                                    int n_competitors)
        : generic_decoder("tpc_decoder_aff3ct"),
        d_K_sqrt(K_sqrt),
        d_N_sqrt(N_sqrt)
    {
        set_frame_size(K_sqrt);
        int N = N_sqrt*N_sqrt;
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

        d_pi = std::make_unique<aff3ct::module::Interleaver<Q_8>>(*d_interleaver_core);

        d_quant = std::make_unique<aff3ct::module::Quantizer_pow2_fast<float, Q_8>>(N, 2);
        d_quant_input = std::vector<Q_8>(N);
        d_tmp_input = std::vector<float>(N);

        d_poly_gen = std::make_unique<aff3ct::tools::BCH_polynomial_generator<B_8>>(N_sqrt, t);
        int rdncy = d_poly_gen->get_n_rdncy();
        d_K_sqrt = N_sqrt - rdncy;
        auto dec_BCH = aff3ct::module::Decoder_BCH_std<B_8, Q_8>(d_K_sqrt, N_sqrt, *d_poly_gen);
        auto enc_BCH = aff3ct::module::Encoder_BCH<B_8>(d_K_sqrt, N_sqrt, *d_poly_gen);
        dec_BCH.set_n_frames(N_sqrt);
        enc_BCH.set_n_frames(N_sqrt);
        auto cp_r = aff3ct::module::Decoder_chase_pyndiah<B_8, Q_8>(d_K_sqrt, N_sqrt, dec_BCH, enc_BCH);
        auto cp_c = aff3ct::module::Decoder_chase_pyndiah<B_8, Q_8>(d_K_sqrt, N_sqrt, dec_BCH, enc_BCH);
        cp_r.set_n_frames(N_sqrt);
        cp_c.set_n_frames(N_sqrt);

        d_decoder = std::make_unique<aff3ct::module::Decoder_turbo_product<B_8, Q_8>>(n_iterations, alpha, cp_r, cp_c, *d_pi);
        

//         Decoder_chase_pyndiah(const int K,
//                           const int N, // N with the parity bit if any
//                           const Decoder_BCH<B, R>& dec,
//                           const Encoder<B>& enc,
//                           const int n_least_reliable_positions = 2,
//                           const int n_test_vectors = 0,
//                           const int n_competitors = 0,
//                           const std::vector<float>& cp_coef = { 1, 1, 1, 1, 0 }); // the a b c d and e coef
//   public:
//     Decoder_turbo_product(const int& n_ite,
//                           const std::vector<float>& alpha,
//                           const Decoder_chase_pyndiah<B, R>& cp_r,
//                           const Decoder_chase_pyndiah<B, R>& cp_c,
//                           const Interleaver<R>& pi,
//                           const std::vector<float>& beta = {});
}

tpc_decoder_aff3ct_impl::~tpc_decoder_aff3ct_impl()
{
}

int tpc_decoder_aff3ct_impl::get_output_size() { return d_K_sqrt * d_K_sqrt; }
int tpc_decoder_aff3ct_impl::get_input_size() { return d_N_sqrt * d_N_sqrt; }

bool tpc_decoder_aff3ct_impl::set_frame_size(unsigned int K)
{
    return true;
}

double tpc_decoder_aff3ct_impl::rate() { return static_cast<float>(d_K_sqrt * d_K_sqrt) / d_N_sqrt * d_N_sqrt; } // decoder rate

void tpc_decoder_aff3ct_impl::generic_work(const void* inbuffer, void* outbuffer)
{
    const float* in = (const float*)inbuffer;
    B_8* out = (B_8*)outbuffer;

    volk_32f_s32f_multiply_32f(d_tmp_input.data(), in, -1.0f, d_N_sqrt*d_N_sqrt);
    d_quant->process(d_tmp_input.data(), d_quant_input.data(), -1);
    
    int status = d_decoder->decode_siho(d_quant_input.data(), out, -1);
    if (status == 1) {
        std::cout << "Decoding FAILURE" << std::endl;
    }
}

} /* namespace fec_dev */
} /* namespace gr */