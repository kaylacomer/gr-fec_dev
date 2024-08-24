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
#include <gnuradio/fec_dev/aff3ct_decoder.h>

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
    * \param frame_bits Number of encoded bits per frame.
    * \param t Number of correctable errors
    * \param quant_fixed_point_pos Position of decimal point in quantized representation
    * \param quant_saturation_pos Quantizer's saturation position
    * \param quant_impl Quantizer implementation - STD/FAST/NO
    * \param dec_impl Decoder implementation: fast or standard
    * \param simd_strat SIMD strategy for BCH Encoder. Only need to specify if using GENIUS dec_impl. SEQ or INTER
    */
    static generic_decoder::sptr make(int frame_bits, uint8_t t=5, uint8_t quant_fixed_point_pos = 2, uint8_t quant_saturation_pos = 6,
                    Quantizer::quantizer_impl_t quant_impl=Quantizer::STD, Decoder::decoder_impl_t dec_impl=Decoder::STD, SIMD::simd_strat_t simd_strat=SIMD::SEQ);

    /*!
    * Sets the uncoded frame size to \p frame_size
    */
    bool set_frame_size(unsigned int frame_bits) override = 0;

    /*!
    * Returns the coding rate of this decoder.
    */
    double rate() override = 0;
};

} // namespace fec_dev
} // namespace gr

#endif /* INCLUDED_FEC_DEV_BCH_DECODER_H */
