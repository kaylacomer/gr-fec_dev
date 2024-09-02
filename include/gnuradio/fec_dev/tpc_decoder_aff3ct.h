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
* \brief TPC decoding class (via AFF3CT library).
* \ingroup error_coding_blk
*
* \details
* This class performs Turbo Product Code (TPC)
* decoding using the AFF3CT library API. For more information about the decoder parameters, see
* https://aff3ct.readthedocs.io/en/latest/user/simulation/parameters/codec/turbo_prod/codec.html
* For more information about the quantization parameters, see
* https://aff3ct.readthedocs.io/en/latest/user/simulation/parameters/quantizer/quantizer.html.
*
* The TPC decoding class is currently minimally functional and may not be configurable to
* the user's needs. This class needs work
*/
class FEC_API tpc_decoder_aff3ct : virtual public fec::generic_decoder
{
public:
    /*!
    * Build a tpc decoding FEC API object.
    *
    * \param K_sqrt Number of bits per frame input to each BCH sub-encoder / output from each sub-decoder
    * \param N_sqrt Number of bits per frame output from each BCH sub-encoder / input to each sub-decoder
    * \param t Correction factor in each BCH sub-encoder
    * \param bch_simd_strat SIMD strategy for BCH sub-encoders. Only works with SEQ currently.
    * \param interleaver Type of interleaver
    * \param read_order Interleaver read order for COL_ROW, ROW_COL types
    * \param parity_extended Whether the sub-encoders have a parity bit
    * \param quant_fixed_point_pos Position of decimal point in quantized representation
    * \param quant_saturation_pos Quantizer's saturation position
    * \param quant_impl Quantizer implementation: STD or FAST
    * \param bch_dec_impl Sub-decoder implementation: STD/FAST/GENIUS. Only tested with STD
    * \param chase_pyndiah_impl Chase-Pyndiah decoder implementation: STD/FAST. Only tested with STD
    * \param n_iterations Number of iterations in decoding process
    * \param alpha Weighting factor, twice the number of iterations
    * \param beta Reliability factor. If not given, dynamically computed. NOT currently implemented
    * \param chase_pyndiah_coef 5 CP constant coefficients, a, b, c, d, e
    * \param n_least_reliable_pos Number of least reliable positions
    * \param n_test_vectors Number of test vectors
    * \param n_competitors Number of competitors. NOT currently implemented
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
