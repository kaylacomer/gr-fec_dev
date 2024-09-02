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
* \brief BCH Decoding class (via AFF3CT library).
* \ingroup error_coding_blk
*
* \details
* This class performs Bose, Ray-Chaudhuri, and Hocquenghem (BCH) 
* decoding using the AFF3CT library API. For more information about the decoder parameters, see
* https://aff3ct.readthedocs.io/en/latest/user/simulation/parameters/codec/bch/codec.html.
* For more information about the quantization parameters, see
* https://aff3ct.readthedocs.io/en/latest/user/simulation/parameters/quantizer/quantizer.html.
* 
* The AFF3CT BCH API requires codeword size N = 2^m - 1, where m is an integer from 3.
* The constructor calculates the lowest possible m for the user-specified \p frame_bits and \p t. The
* decoder pads the input with zeros if \p frame_bits is less than \p K, which is likely unless
* the user has chosen values to avoid zero-padding. Currently, zero-padding results in
* last 7 bits of the output being zero instead of the correct, initial values. It is
* recommended that the user choose \p frame_bits and \p t such that zero-padding is not required.
* Appropriate values for \p frame_bits and \p t can be found in the file names of the AFF3CT reference
* simulation data: https://github.com/aff3ct/error_rate_references/tree/51dff02f0fac4840be7d41c1a01a56e206cb5f07/BCH/AWGN/BPSK/ALGEBRAIC.
*
* The BCH decoder implementation \p dec_impl can be 'STD', 'FAST', or 'GENIUS'. A user may
* need to modify the source code if they intend to use 'FAST' or 'GENIUS' implementation;
* these options have not been well-tested. Notes on decoder implementation from the AFF3CT
* User Manual:
* \li 'STD': A standard implementation of the BCH.
* \li 'FAST': Select the fast implementation optimized for SIMD architectures.
* \li 'GENIUS': A really fast implementation that compare the input to the original codeword and
* correct it only when the number of errors is less or equal to the BCH correction power.
* In the STD implementation, the Chien search finds roots of the error location polynomial.
* If the number of found roots does not match the number of found errors by the BM algorithm,
* then the frame is not modified.
* However, in the FAST implementation the correction of the bits is done at the same time as
* the execution of the Chien search. Then when the latter fails, the frame can be modified.
* When a frame is very corrupted and when the above STD and FAST implementations can be wrong
* in the correction by converging to another codeword, the GENIUS implementation cannot fail.
* Results may then differ from a real word implementation.
*/
class FEC_API bch_decoder : virtual public fec::generic_decoder
{
public:
    /*!
    * Build a BCH decoding FEC API object.
    *
    * \param frame_bits Number of bits per frame
    * \param t Number of correctable errors / correction power of the decoder
    * \param quant_fixed_point_pos Position of decimal point in quantized representation
    * \param quant_saturation_pos Number of bits used in the fixed-point representation
    * \param quant_impl Quantizer implementation - STD/FAST/NO
    * \param dec_impl Decoder implementation: STD/FAST/GENIUS
    * \param simd_strat SIMD strategy for BCH Encoder. Only need to specify if using GENIUS \p dec_impl. SEQ/INTER
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
