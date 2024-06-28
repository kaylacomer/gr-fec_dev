/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_BCH_DECODER_H
#define INCLUDED_FEC_DEV_BCH_DECODER_H

#include <gnuradio/fec_dev/api.h>
#include <gnuradio/fec/generic_decoder.h>
#include <aff3ct.hpp>

namespace gr {
namespace fec_dev {

/*!
* \brief bch Decoding class.
* \ingroup error_coding_blk
*
* \details
* A bch decoder class
*/
class FEC_API bch_decoder : virtual public fec::generic_decoder
{
public:
    /*!
    * Build a bch decoding FEC API object.
    *
    * \param codeword Number of encoded bits per frame.
    */
    static generic_decoder::sptr make(int codeword=127, uint8_t t=5);

    /*!
    * Sets the uncoded frame size to \p frame_size. If \p
    * frame_size is greater than the value given to the
    * constructor, the frame size will be capped by that initial
    * value and this function will return false. Otherwise, it
    * returns true.
    */
    bool set_frame_size(unsigned int codeword) override = 0;

    /*!
    * Returns the coding rate of this decoder.
    */
    double rate() override = 0;
};

} // namespace fec_dev
} // namespace gr

#endif /* INCLUDED_FEC_DEV_BCH_DECODER_H */
