/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_TPC_ENCODER_AFF3CT_H
#define INCLUDED_FEC_DEV_TPC_ENCODER_AFF3CT_H

#include <gnuradio/fec_dev/api.h>
#include <gnuradio/fec/generic_encoder.h>
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
*/
class FEC_API tpc_encoder_aff3ct : virtual public fec::generic_encoder
{
public:
    /*!
    * Build a TPC encoding FEC API object.
    *
    * \param K_sqrt Number of bits per frame input to each BCH sub-encoder / output from each sub-decoder
    * \param N_sqrt Number of bits per frame output from each BCH sub-encoder / input to each sub-decoder
    * \param t Correction factor in each BCH sub-encoder
    * \param bch_simd_strat SIMD strategy for BCH sub-encoders. Only works with SEQ currently.
    * \param interleaver Type of interleaver
    * \param read_order Interleaver read order for COL_ROW, ROW_COL types
    * \param parity_extended Whether the sub-encoders have a parity bit
    */
    static generic_encoder::sptr make(int K_sqrt,
                                    int N_sqrt,
                                    int t,
                                    SIMD::simd_strat_t bch_simd_strat=SIMD::SEQ,
                                    Interleaver::interleaver_t interleaver=Interleaver::ROW_COL,
                                    Interleaver::itl_read_order_t read_order=Interleaver::TOP_LEFT,
                                    bool parity_extended=false);

    /*!
    * Sets the uncoded frame size to \p frame_size
    */
    bool set_frame_size(unsigned int frame_size) override = 0;

    /*!
    * Returns the coding rate of this encoder.
    */
    double rate() override = 0;
};

} // namespace fec_dev
} // namespace gr

#endif /* INCLUDED_FEC_DEV_TPC_ENCODER_AFF3CT_H */
