/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_POLAR_DECODER_AFF3CT_H
#define INCLUDED_FEC_DEV_POLAR_DECODER_AFF3CT_H

#include <gnuradio/fec_dev/api.h>
#include <gnuradio/fec/generic_decoder.h>

#include <gnuradio/fec_dev/aff3ct_decoder.h>
#include <gnuradio/fec_dev/polar_common_aff3ct.h>

namespace gr {
namespace fec_dev {

/*!
* \brief polar decoding class.
* \ingroup error_coding_blk
*
* \details
* A polar decoder class
*/
class FEC_API polar_decoder_aff3ct : virtual public fec::generic_decoder
{
public:
    /*!
    * Build a polar decoding FEC API object.
    *
    * \param frame_size Number of bits per frame
    */
    static generic_decoder::sptr make(int K,
                            int N,
                            Polar::frozen_bit_gen_t frozen_bit_gen = Polar::GA_ARIKAN,
                            Polar::noise_t noise_type = Polar::Sigma,
                            Polar::decoder_t decoder_type = Polar::SC,
                            Decoder::decoder_impl_t dec_impl = Decoder::NAIVE,
                            uint8_t quant_fixed_point_pos = 2,
                            uint8_t quant_saturation_pos = 6,
                            Quantizer::quantizer_impl_t quant_impl=Quantizer::STD);

    /*!
    * Sets the uncoded frame size to \p frame_size
    */
    bool set_frame_size(unsigned int frame_size) override = 0;

    /*!
    * Returns the coding rate of this decoder.
    */
    double rate() override = 0;
};

} // namespace fec_dev
} // namespace gr

#endif /* INCLUDED_FEC_DEV_POLAR_DECODER_AFF3CT_H */
