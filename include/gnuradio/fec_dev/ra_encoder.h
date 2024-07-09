/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_RA_ENCODER_H
#define INCLUDED_FEC_DEV_RA_ENCODER_H

#include <gnuradio/fec_dev/api.h>
#include <gnuradio/fec/generic_encoder.h>

namespace gr {
namespace fec_dev {

/*!
* \brief ra Encoding class.
* \ingroup error_coding_blk
*
* \details
* A ra encoder class
*/
class FEC_API ra_encoder : virtual public fec::generic_encoder
{
public:
    /*!
    * Build a ra encoding FEC API object.
    *
    * \param K Number of bits per frame. If using in the
    *        tagged stream style, this is the maximum allowable
    *        number of bits per frame.
    */
    static generic_encoder::sptr make(int K, int N);

    /*!
    * Sets the uncoded frame size to \p K. If \p
    * K is greater than the value given to the
    * constructor, the frame size will be capped by that initial
    * value and this function will return false. Otherwise, it
    * returns true.
    */
    bool set_frame_size(unsigned int K) override = 0;

    /*!
    * Returns the coding rate of this encoder.
    */
    double rate() override = 0;
};

} // namespace fec_dev
} // namespace gr

#endif /* INCLUDED_FEC_DEV_RA_ENCODER_H */
