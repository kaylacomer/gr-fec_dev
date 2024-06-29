/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_RA_DECODER_H
#define INCLUDED_FEC_DEV_RA_DECODER_H

#include <gnuradio/fec_dev/api.h>
#include <gnuradio/fec/generic_decoder.h>

namespace gr {
namespace fec_dev {

/*!
* \brief ra Encoding class.
* \ingroup error_coding_blk
*
* \details
* A ra decoder class
*/
class FEC_API ra_decoder : virtual public fec::generic_decoder
{
public:
    /*!
    * Build a ra decoding FEC API object.
    *
    * \param frame_size Number of bits per frame. If using in the
    *        tagged stream style, this is the maximum allowable
    *        number of bits per frame.
    */
    static generic_decoder::sptr make(int frame_size);

    /*!
    * Sets the uncoded frame size to \p frame_size. If \p
    * frame_size is greater than the value given to the
    * constructor, the frame size will be capped by that initial
    * value and this function will return false. Otherwise, it
    * returns true.
    */
    bool set_frame_size(unsigned int frame_size) override = 0;

    /*!
    * Returns the coding rate of this decoder.
    */
    double rate() override = 0;
};


} // namespace fec_dev
} // namespace gr

#endif /* INCLUDED_FEC_DEV_RA_DECODER_H */
