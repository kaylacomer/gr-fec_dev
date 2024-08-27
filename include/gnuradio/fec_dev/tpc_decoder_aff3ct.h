/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_TPC_DECODER_AFF3CT_H
#define INCLUDED_FEC_DEV_TPC_DECODER_AFF3CT_H

#include <gnuradio/fec_dev/api.h>
#include <gnuradio/fec/generic_decoder.h>
#include "gnuradio/fec_dev/aff3ct_interleaver.h"
#include "gnuradio/fec_dev/aff3ct_decoder.h"

namespace gr {
namespace fec_dev {

/*!
* \brief tpc decoding class.
* \ingroup error_coding_blk
*
* \details
* A tpc decoder class
*/
class FEC_API tpc_decoder_aff3ct : virtual public fec::generic_decoder
{
public:
    /*!
    * Build a tpc decoding FEC API object.
    *
    * \param frame_size Number of bits per frame
    */
    static generic_decoder::sptr make(int K_sqrt,
                          int N_sqrt,
                          int t,
                          SIMD::simd_strat_t bch_simd_strat=SIMD::SEQ,
                          Interleaver::interleaver_t interleaver=Interleaver::ROW_COL,
                          Interleaver::itl_read_order_t read_order=Interleaver::TOP_LEFT,
                          bool parity_extended=false,
                          uint8_t quant_fixed_point_pos = 2,
                          uint8_t quant_saturation_pos = 6,
                          Quantizer::quantizer_impl_t quant_impl=Quantizer::STD,
                          Decoder::decoder_impl_t bch_dec_impl=Decoder::STD,
                          Decoder::decoder_impl_t chase_pyndiah_impl=Decoder::STD,
                          int n_iterations = 4,
                          std::vector<float> alpha = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5},
                          std::vector<float> beta = {},
                          std::vector<float> chase_pyndiah_coef = {1, 1, 1, 1, 0},
                          int n_least_reliable_pos = 2,
                          int n_test_vectors = 4,
                          int n_competitors = 4);

    /*!
    * Sets the uncoded frame size to \p frame_size
    */
    bool set_frame_size(unsigned int frame_size) override = 0;

    /*!
    * Returns the coding rate of this decoder.
    */
    double rate() override = 0;
};

} // namespace fec_dev
} // namespace gr

#endif /* INCLUDED_FEC_DEV_TPC_DECODER_AFF3CT_H */
