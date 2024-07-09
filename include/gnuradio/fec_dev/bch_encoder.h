/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_BCH_ENCODER_H
#define INCLUDED_FEC_DEV_BCH_ENCODER_H

#include <gnuradio/fec_dev/api.h>
#include <gnuradio/fec/generic_encoder.h>
#include <aff3ct.hpp>

namespace gr {
namespace fec_dev {

/*!
* \brief bch Encoding class.
* \ingroup error_coding_blk
*
* \details
* A bch encoder class
*/
class FEC_API bch_encoder : virtual public fec::generic_encoder
{
public:
    /*!
    * Build a bch encoding FEC API object.
    *
    * \param frame_size Number of bits per frame. If using in the
    *        tagged stream style, this is the maximum allowable
    *        number of bits per frame.
    */
    static generic_encoder::sptr make(int frame_bits=127, uint8_t t=5);

    /*!
    * Sets the uncoded frame size to \p frame_size. If \p
    * frame_size is greater than the value given to the
    * constructor, the frame size will be capped by that initial
    * value and this function will return false. Otherwise, it
    * returns true.
    */
    bool set_frame_size(unsigned int frame_bits) override = 0;

    /*!
    * Returns the coding rate of this encoder.
    */
    double rate() override = 0;
};

} // namespace fec_dev
} // namespace gr

#endif /* INCLUDED_FEC_DEV_BCH_ENCODER_H */
