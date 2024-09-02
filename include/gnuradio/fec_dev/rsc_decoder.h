/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_RSC_DECODER_AFF3CT_H
#define INCLUDED_FEC_DEV_RSC_DECODER_AFF3CT_H

#include <gnuradio/fec_dev/api.h>
#include <gnuradio/fec/generic_decoder.h>
#include <gnuradio/fec_dev/aff3ct_decoder.h>
#include <gnuradio/fec_dev/bcjr.h>

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
* For more information about the quantization parameters, see
* https://aff3ct.readthedocs.io/en/latest/user/simulation/parameters/quantizer/quantizer.html.
*/
class FEC_API rsc_decoder : virtual public fec::generic_decoder
{
public:
    /*!
    * Build an RSC decoding FEC API object.
    *
    * \param K Number of bits per frame
    * \param polys Polynomials that define the encoder. Specified in octal
    * \param trellis_size Number of trellis stages
    * \param buffered Whether to use buffered encoding. Impacts bit organization
    * \param dec_type Only tested for BCJR. Needs work to use Viterbi
    * \param quant_fixed_point_pos Position of decimal point in quantized representation
    * \param quant_saturation_pos Quantizer's saturation position
    * \param quant_impl Quantizer implementation: STD or FAST
    * \param bcjr_impl Only tested with GENERIC. Needs work to use STD, FAST, or VERY_FAST
    * \param simd_strat Only tested with SEQ. Needs work to use INTER, INTRA, or INTER_INTRA
    * \param simd_interintra_impl For \p simd_strat INTER_INTRA, which has not been implemented
    * \param viterbi_list_poly_key Polynomial string key (ex. "32-GZIP") matching list in bcjr.h. Not implemented yet
    * \param viterbi_siho_is_closed is_closed option in Viterbi constructors. Not implemented yet
    * 
    */
    static generic_decoder::sptr make(int K, 
                  std::vector<int> polys={013,015}, 
                  int trellis_size = 8,
                  bool buffered=true,
                  RSC::rsc_decoder_impl_t dec_type = RSC::BCJR,
                  uint8_t quant_fixed_point_pos = 1,
                  uint8_t quant_saturation_pos = 6,
                  Quantizer::quantizer_impl_t quant_impl=Quantizer::STD,
                  BCJR::bcjr_impl_t bcjr_impl=BCJR::GENERIC,
                  SIMD::simd_strat_t simd_strat=SIMD::SEQ,
                  SIMD::simd_interintra_impl_t simd_interintra_impl=SIMD::NA,
                  std::string viterbi_list_poly_key="none",
                  bool viterbi_siho_is_closed=true);

    /*!
    * Sets the uncoded frame size to \p frame_size
    */
    bool set_frame_size(unsigned int K) override = 0;

    /*!
    * Returns the coding rate of this decoder.
    */
    double rate() override = 0;
};

} // namespace fec_dev
} // namespace gr

#endif /* INCLUDED_FEC_DEV_RSC_DECODER_AFF3CT_H */
