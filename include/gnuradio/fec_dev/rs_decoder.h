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
* \brief RS Decoding class (via AFF3CT library). NOT FUNCTIONAL
* \ingroup error_coding_blk
*
* \details
* This class performs Reed-Solomon (RS)
* decoding using the AFF3CT library API. For more information about the decoder parameters, see
* https://aff3ct.readthedocs.io/en/latest/user/simulation/parameters/codec/rs/codec.html.
* For more information about the quantization parameters, see
* https://aff3ct.readthedocs.io/en/latest/user/simulation/parameters/quantizer/quantizer.html.
*
* The AFF3CT RS API requires codeword size N = 2^m - 1, where m is an integer from 3 that also
* represents the number of bits per symbol. Thus, the binary codeword size is N x m.
* The constructor calculates the lowest possible m for the user-specified \p frame_size and \p t. The
* decoder pads the input with zeros if \p frame_size is less than \p K, which is likely unless
* the user has chosen values to avoid zero-padding.
* 
* Note: This class is NOT currently functional and needs work.
*/
class FEC_API rs_decoder : virtual public fec::generic_decoder
{
public:
    /*!
    * Build an RS decoding FEC API object.
    *
    * \param frame_size Number of bits per frame
    * \param t Number of correctable errors / correction power of the decoder
    * \param quant_fixed_point_pos Position of decimal point in quantized representation
    * \param quant_saturation_pos Number of bits used in the fixed-point representation
    * \param quant_impl Quantizer implementation - STD/FAST/NO
    * \param dec_impl Decoder implementation: STD/GENIUS
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
