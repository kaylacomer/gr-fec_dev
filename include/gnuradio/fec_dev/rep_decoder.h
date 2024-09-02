/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_REP_DECODER_H
#define INCLUDED_FEC_DEV_REP_DECODER_H

#include <gnuradio/fec_dev/api.h>
#include <gnuradio/fec/generic_decoder.h>
#include "gnuradio/fec_dev/aff3ct_decoder.h"

namespace gr {
namespace fec_dev {

/*!
* \brief Repetition Decoding class (via AFF3CT library). NOT FUNCTIONAL
* \ingroup error_coding_blk
*
* \details
* This class performs Low-Density Parity-Check (LDPC)
* decoding using the AFF3CT library API. For more information about the decoder parameters, see
* https://aff3ct.readthedocs.io/en/latest/user/simulation/parameters/codec/rep/codec.html.
* For more information about the quantization parameters, see
* https://aff3ct.readthedocs.io/en/latest/user/simulation/parameters/quantizer/quantizer.html.
* 
* Note: This class is NOT currently functional and needs work.
*/
class FEC_API rep_decoder : virtual public fec::generic_decoder
{
public:
    /*!
    * Build a Repetition decoding FEC API object.
    *
    * \param frame_size Number of bits per frame
    * \param rep Number of repetitions per bit. Codeword size N = rep*K
    * \param buffered Buffered encoding. Impacts encoded bit organization
    * \param quant_fixed_point_pos Position of decimal point in quantized representation
    * \param quant_saturation_pos Quantizer's saturation position
    * \param quant_impl Quantizer implementation: STD or FAST
    */
    static generic_decoder::sptr make(int frame_size, int rep=3, bool buffered = true, uint8_t quant_fixed_point_pos=2, uint8_t quant_saturation_pos=6,
    Quantizer::quantizer_impl_t quant_impl=Quantizer::quantizer_impl_t::STD, Decoder::decoder_impl_t dec_impl=Decoder::decoder_impl_t::STD);

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

#endif /* INCLUDED_FEC_DEV_REP_DECODER_H */
