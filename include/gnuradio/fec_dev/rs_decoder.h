/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_RS_DECODER_H
#define INCLUDED_FEC_DEV_RS_DECODER_H

#include <gnuradio/fec_dev/api.h>
#include <gnuradio/fec/generic_decoder.h>
#include <gnuradio/fec_dev/aff3ct_decoder.h>

namespace gr {
namespace fec_dev {

/*!
* \brief rs Encoding class.
* \ingroup error_coding_blk
*
* \details
* A rs decoder class
*/
class FEC_API rs_decoder : virtual public fec::generic_decoder
{
public:
    /*!
    * Build a rs decoding FEC API object.
    *
    * \param frame_size Number of bits per frame. If using in the
    *        tagged stream style, this is the maximum allowable
    *        number of bits per frame.
    */
    static generic_decoder::sptr make(int frame_size, uint8_t t=5, uint8_t quant_fixed_point_pos = 2, uint8_t quant_saturation_pos = 6,
                    Quantizer::quantizer_impl_t quant_impl=Quantizer::STD, Decoder::decoder_impl_t dec_impl=Decoder::STD);

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

#endif /* INCLUDED_FEC_DEV_RS_DECODER_H */
