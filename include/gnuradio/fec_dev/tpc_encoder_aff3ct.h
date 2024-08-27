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
* \brief tpc Encoding class.
* \ingroup error_coding_blk
*
* \details
* A tpc encoder class
*/
class FEC_API tpc_encoder_aff3ct : virtual public fec::generic_encoder
{
public:
    /*!
    * Build a tpc encoding FEC API object.
    *
    * \param frame_size Number of bits per frame
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
