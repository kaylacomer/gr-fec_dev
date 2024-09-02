/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_RSC_ENCODER_AFF3CT_H
#define INCLUDED_FEC_DEV_RSC_ENCODER_AFF3CT_H

#include <gnuradio/fec_dev/api.h>
#include <gnuradio/fec/generic_encoder.h>

namespace gr {
namespace fec_dev {

/*!
* \brief RSC decoding class (via AFF3CT library).
* \ingroup error_coding_blk
*
* \details
* This class performs Recursive Systematic Convolutional (RSC)
* decoding using the AFF3CT library API. For more information about the decoder parameters, see
* https://aff3ct.readthedocs.io/en/latest/user/simulation/parameters/codec/rsc/codec.html.
*/
class FEC_API rsc_encoder : virtual public fec::generic_encoder
{
public:
    /*!
    * Build an RSC encoding FEC API object.
    *
    * \param K Number of bits per frame
    * \param polys Polynomials that define the encoder. Specified in octal
    * \param trellis_size Number of trellis stages
    * \param buffered Whether to use buffered encoding. Impacts bit organization
    */
    static generic_encoder::sptr make(int frame_size, std::vector<int> polys={013,015}, int trellis_size = 8, bool buffered=true);

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

#endif /* INCLUDED_FEC_DEV_RSC_ENCODER_AFF3CT_H */
