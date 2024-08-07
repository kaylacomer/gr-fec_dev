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
* \brief Repeat-Accumulate Encoding class.
* \ingroup error_coding_blk
*
* \details
* Repeat-Accumulate encoder class
*/
class FEC_API ra_encoder : virtual public fec::generic_encoder
{
public:
    /*!
    * Build an RA encoding FEC API object.
    *
    * \param K Number of bits per frame
    * \param rep Number of repetitions per bit. Codeword size N = rep*K
    */
    static generic_encoder::sptr make(int K, int rep=3);

    /*!
    * Sets the uncoded frame size to \p K
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
