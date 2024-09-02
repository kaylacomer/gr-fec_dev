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
* \brief Polar decoding class (via AFF3CT library).
* \ingroup error_coding_blk
*
* \details
* This class performs Polar
* decoding using the AFF3CT library API. For more information about the decoder parameters, see
* https://aff3ct.readthedocs.io/en/latest/user/simulation/parameters/codec/polar/codec.html.
* For more information about the quantization parameters, see
* https://aff3ct.readthedocs.io/en/latest/user/simulation/parameters/quantizer/quantizer.html.
* 
* The polar decoding class is currently minimally functional and may not be configurable to
* the user's needs. This class needs work
*/
class FEC_API polar_decoder_aff3ct : virtual public fec::generic_decoder
{
public:
    /*!
    * Build polar decoding FEC API object. Many parameters only have one option
    * that has been implemented. If the user tries to use an option
    * that is not configured, they will encounter a standard runtime error.
    *
    * \param K Number of bits per frame output from decoder
    * \param N Number of bits per frame input to the decoder
    * \param sigma Standard deviation of noise
    * \param frozen_bit_gen Only GA_ARIKAN implemented. Options: GA_ARIKAN, FIVE_G, BEC, GA, TV
    * \param noise_type Only Sigma implemented. Options: Sigma, Received_optical_power, Event_probability
    * \param decoder_type Only SC implemented. Options: SC, ASCL, SCAN, SCF, SCL
    * \param dec_impl Only NAIVE implemented. Options: NAIVE, FAST
    * \param quant_fixed_point_pos Position of decimal point in quantized representation
    * \param quant_saturation_pos Number of bits used in the fixed-point representation
    * \param quant_impl Quantizer implementation - STD/FAST/NO
    */
    static generic_decoder::sptr make(int K,
                            int N,
                            float sigma,
                            Polar::frozen_bit_gen_t frozen_bit_gen = Polar::GA_ARIKAN,
                            Polar::noise_t noise_type = Polar::Sigma,
                            Polar::decoder_t decoder_type = Polar::SC,
                            Decoder::decoder_impl_t dec_impl = Decoder::NAIVE,
                            uint8_t quant_fixed_point_pos = 1,
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
