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
#include <gnuradio/fec_dev/aff3ct_decoder.h>

namespace gr {
namespace fec_dev {

/*!
* \brief BCH Encoding class (via AFF3CT library).
* \ingroup error_coding_blk
*
* \details
* This class performs Bose, Ray-Chaudhuri, and Hocquenghem (BCH) 
* encoding using the AFF3CT library API. For more information about the encoder parameters, see
* https://aff3ct.readthedocs.io/en/latest/user/simulation/parameters/codec/bch/codec.html.
* 
* The AFF3CT BCH API requires codeword size N = 2^m - 1, where m is an integer from 3.
* The constructor calculates the lowest possible m for the user-specified \p K and \p t. The
* decoder pads the input with zeros if \p K is less than \p frame_bits, which is likely unless
* the user has chosen values to avoid zero-padding. Currently, zero-padding results in
* last 7 bits of the output being zero instead of the correct, initial values. It is
* recommended that the user choose \p K and \p t such that zero-padding is not required.
* Appropriate values for \p K and \p t can be found in the file names of the AFF3CT reference
* simulation data: https://github.com/aff3ct/error_rate_references/tree/51dff02f0fac4840be7d41c1a01a56e206cb5f07/BCH/AWGN/BPSK/ALGEBRAIC.
*
* Single Instruction Multiple Data (SIMD) parameter \p simd_strat may not work when set to 'INTER'
* and has only been tested without SIMD ('SEQ' setting). For more information on SIMD,
* see the AFF3CT User Manual.
*/
class FEC_API bch_encoder : virtual public fec::generic_encoder
{
public:
    /*!
    * Build a bch encoding FEC API object.
    *
    * \param frame_bits Number of bits per frame
    * \param t Number of correctable errors / correction power of the decoder
    * \param simd_strat Single Instruction Multiple Data (SIMD) strategy, default SEQ
    */
    static generic_encoder::sptr make(int frame_bits, uint8_t t=5, SIMD::simd_strat_t simd_strat=SIMD::SEQ);

    /*!
    * Sets the uncoded frame size to \p frame_size
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
