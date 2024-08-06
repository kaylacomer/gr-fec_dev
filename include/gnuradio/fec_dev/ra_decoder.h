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
* \brief Repeat-Accumulate Decoding class.
* \ingroup error_coding_blk
*
* \details
* Repeat-Accumulate decoder class
*/
class FEC_API ra_decoder : virtual public fec::generic_decoder
{
public:
    /*!
    * Build an RA decoding FEC API object.
    *
    * \param K Number of bits per frame
    * \param rep Number of repetitions per bit. Codeword size N = rep*K
    * \param iter Number of iterations to perform in the decoder
    */
    static generic_decoder::sptr make(int K, int rep=3, int iter=1);

    /*!
    * Sets the uncoded frame size to \p K
    */
    bool set_frame_size(unsigned int K) override = 0;

    /*!
    * Returns the coding rate of this decoder.
    */
    double rate() override = 0;
};


} // namespace fec_dev
} // namespace gr

#endif /* INCLUDED_FEC_DEV_RA_DECODER_H */
