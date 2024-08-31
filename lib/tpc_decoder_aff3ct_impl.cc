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
        d_K(45),
        d_N(63*63)
    {
        set_frame_size(K_sqrt);

        d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_row_column<>>(d_N, std::sqrt(d_N), "TOP_LEFT");
        d_pi = std::make_unique<aff3ct::module::Interleaver<Q_8>>(*d_interleaver_core);

        d_quant = std::make_unique<aff3ct::module::Quantizer_pow2_fast<float, Q_8>>(d_N, 2);
        d_quant_input = std::vector<Q_8>(d_N);
        d_tmp_input = std::vector<float>(d_N);

        d_poly_gen = std::make_unique<aff3ct::tools::BCH_polynomial_generator<B_8>>(std::sqrt(d_N), t);
        int rdncy = d_poly_gen->get_n_rdncy();
        d_K = std::sqrt(d_N) - rdncy;
        auto dec_BCH = aff3ct::module::Decoder_BCH_std<B_8, Q_8>(d_K, std::sqrt(d_N), *d_poly_gen);
        auto enc_BCH = aff3ct::module::Encoder_BCH<B_8>(d_K, std::sqrt(d_N), *d_poly_gen);
        dec_BCH.set_n_frames(std::sqrt(d_N));
        enc_BCH.set_n_frames(std::sqrt(d_N));
        auto cp_r = aff3ct::module::Decoder_chase_pyndiah<B_8, Q_8>(d_K, std::sqrt(d_N), dec_BCH, enc_BCH);
        auto cp_c = aff3ct::module::Decoder_chase_pyndiah<B_8, Q_8>(d_K, std::sqrt(d_N), dec_BCH, enc_BCH);
        cp_r.set_n_frames(std::sqrt(d_N));
        cp_c.set_n_frames(std::sqrt(d_N));
        // std::vector<float> alpha = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
        d_decoder = std::make_unique<aff3ct::module::Decoder_turbo_product<B_8, Q_8>>(4, alpha, cp_r, cp_c, *d_pi);
        

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

int tpc_decoder_aff3ct_impl::get_output_size() { return d_K; }
int tpc_decoder_aff3ct_impl::get_input_size() { return d_N; }

bool tpc_decoder_aff3ct_impl::set_frame_size(unsigned int K)
{
    return true;
}

double tpc_decoder_aff3ct_impl::rate() { return static_cast<float>(d_K) / d_N; } // decoder rate

void tpc_decoder_aff3ct_impl::generic_work(const void* inbuffer, void* outbuffer)
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

} /* namespace fec_dev */
} /* namespace gr */