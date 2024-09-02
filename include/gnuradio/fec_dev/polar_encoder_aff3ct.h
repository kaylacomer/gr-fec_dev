/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_POLAR_ENCODER_AFF3CT_H
#define INCLUDED_FEC_DEV_POLAR_ENCODER_AFF3CT_H

#include <gnuradio/fec_dev/api.h>
#include <gnuradio/fec/generic_encoder.h>

#include <gnuradio/fec_dev/polar_common_aff3ct.h>

namespace gr {
namespace fec_dev {

/*!
* \brief Polar encoding class (via AFF3CT library).
* \ingroup error_coding_blk
*
* \details
* This class performs Polar
* encoding using the AFF3CT library API. For more information about the encoder parameters, see
* https://aff3ct.readthedocs.io/en/latest/user/simulation/parameters/codec/polar/codec.html.
* 
* The polar encoding class is currently minimally functional and may not be configurable to
* the user's needs. This class needs work
*/
class FEC_API polar_encoder_aff3ct : virtual public fec::generic_encoder
{
public:
    /*!
    * Build polar encoding FEC API object. Many parameters only have one option
    * that has been implemented. If the user tries to use an option
    * that is not configured, they will encounter a standard runtime error.
    *
    * \param K Number of bits per frame output from encoder
    * \param N Number of bits per frame input to the encoder
    * \param sigma Standard deviation of noise
    * \param frozen_bit_gen Only GA_ARIKAN implemented. Options: GA_ARIKAN, FIVE_G, BEC, GA, TV
    * \param noise_type Only Sigma implemented. Options: Sigma, Received_optical_power, Event_probability
    */
    static generic_encoder::sptr make(int K,
                            int N,
                            float sigma,
                            Polar::frozen_bit_gen_t frozen_bit_gen = Polar::GA_ARIKAN,
                            Polar::noise_t noise_type = Polar::Sigma);

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

#endif /* INCLUDED_FEC_DEV_POLAR_ENCODER_AFF3CT_H */
