/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_TURBO_ENCODER_H
#define INCLUDED_FEC_DEV_TURBO_ENCODER_H

#include <gnuradio/fec/api.h>
#include <gnuradio/fec/generic_encoder.h>
#include "gnuradio/fec_dev/turbo_common.h"
#include <map>
#include <string>

namespace gr {
namespace fec_dev {

/*!
* \brief Turbo Encoding class.
* \ingroup error_coding_blk
*/
class FEC_API turbo_encoder : virtual public fec::generic_encoder
{
public:
    /*!
    * Build a turbo encoding FEC API object.
    *
    * \param frame_bits Number of bits per frame
    * \param standard Select a standard (LTE or CCSDS) to
    *        automatically set some parameters
    * \param buffered Buffered encoding. Impacts encoded bit organization
    * \param polys Polynomials that define the two identical RSC sub-encoders.
    *        Set in octal
    * \param trellis_size Number of trellis stages. 8 for LTE, 16 for CCSDS
    * \param subenc_impl RSC subencoder implementation. Determines whether user
    *        sets polynomials or number of D flip flops
    * \param n_ff Number of D flip flops. Only needed for generic subencoder impl
    * \param interleaver Type of interleaver. Only needs to be set if not using LTE or CCSDS standard
    * \param read_order Interleaver read order for COL_ROW, ROW_COL types
    * \param itl_n_cols Interleaver number of columns for column/row types
    */
    static generic_encoder::sptr make(int frame_bits,
                                      Turbo::enc_standard_t standard=Turbo::LTE,
                                      bool buffered=true,
                                      std::vector<int> polys={013,015},
                                      int trellis_size = 8,
                                      Turbo::subenc_implem_t subenc_impl=Turbo::sys,
                                      int n_ff = -1,
                                      Interleaver::interleaver_t interleaver=Interleaver::NO,
                                      Interleaver::itl_read_order_t read_order=Interleaver::NA,
                                      int itl_n_cols = -1);

    /*!
    * Called in the constructor. Inherited and must be overriden.
    */
    bool set_frame_size(unsigned int frame_bits) override = 0;

    /*!
    * Returns the coding rate of this encoder.
    */
    double rate() override = 0;
};
    
} // namespace fec_dev
} // namespace gr

#endif /* INCLUDED_FEC_DEV_TURBO_ENCODER_H */
