/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_RS_ENCODER_H
#define INCLUDED_FEC_DEV_RS_ENCODER_H

#include <gnuradio/fec_dev/api.h>
#include <gnuradio/fec/generic_encoder.h>

namespace gr {
namespace fec_dev {

/*!
* \brief RS Encoding class (via AFF3CT library). NOT FUNCTIONAL
* \ingroup error_coding_blk
*
* \details
* This class performs Reed-Solomon (RS)
* encoding using the AFF3CT library API. For more information about the encoder parameters, see
* https://aff3ct.readthedocs.io/en/latest/user/simulation/parameters/codec/rs/codec.html.
*
* The AFF3CT RS API requires codeword size N = 2^m - 1, where m is an integer from 3 that also
* represents the number of bits per symbol. Thus, the binary codeword size is N x m.
* The constructor calculates the lowest possible m for the user-specified \p frame_size and \p t. The
* decoder pads the input with zeros if \p frame_size is less than \p K, which is likely unless
* the user has chosen values to avoid zero-padding.
* 
* Note: This class is NOT currently functional and needs work.
*/
class FEC_API rs_encoder : virtual public fec::generic_encoder
{
public:
    /*!
    * Build an RS encoding FEC API object.
    *
    * \param frame_size Number of bits per frame
    * \param t Number of correctable errors / correction power of the decoder
    */
    static generic_encoder::sptr make(int frame_size, uint8_t t=5);

    /*!
    * Sets the uncoded frame size to \p frame_size. If \p
    * frame_size is greater than the value given to the
    * constructor, the frame size will be capped by that initial
    * value and this function will return false. Otherwise, it
    * returns true.
    */
    bool set_frame_size(unsigned int frame_size) override = 0;

    /*!
    * Returns the coding rate of this encoder.
    */
    double rate() override = 0;
};


} // namespace fec_dev
} // namespace gr

#endif /* INCLUDED_FEC_DEV_RS_ENCODER_H */
