/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_REP_ENCODER_H
#define INCLUDED_FEC_DEV_REP_ENCODER_H

#include <gnuradio/fec_dev/api.h>
#include <gnuradio/fec/generic_encoder.h>

namespace gr {
namespace fec_dev {

/*!
* \brief Repetition Encoding class.
* \ingroup error_coding_blk
*
* \details
* A Repetition encoder class
*/
class FEC_API rep_encoder : virtual public fec::generic_encoder
{
public:
    /*!
    * Build a Repetition encoding FEC API object.
    *
    * \param frame_size Number of bits per frame
    * \param rep Number of repetitions per bit. Codeword size N = rep*K
    */
    static generic_encoder::sptr make(int frame_size, int rep=3, bool buffered=true);

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

#endif /* INCLUDED_FEC_DEV_REP_ENCODER_H */
