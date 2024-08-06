/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_LDPC_ENCODER_AFF3CT_H
#define INCLUDED_FEC_DEV_LDPC_ENCODER_AFF3CT_H

#include <gnuradio/fec_dev/api.h>
#include <gnuradio/fec/generic_encoder.h>

namespace gr {
namespace fec_dev {

/*!
* \brief ldpc Encoding class.
* \ingroup error_coding_blk
*
* \details
* A ldpc encoder class
*/
class FEC_API ldpc_encoder_aff3ct : virtual public fec::generic_encoder
{
public:
    /*!
    * Build a ldpc encoding FEC API object.
    *
    * \param frame_size Number of bits per frame
    */
    static generic_encoder::sptr make(int frame_size);

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

#endif /* INCLUDED_FEC_DEV_LDPC_ENCODER_AFF3CT_H */
