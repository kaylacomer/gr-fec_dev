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
* \brief rsc decoding class.
* \ingroup error_coding_blk
*
* \details
* A rsc decoder class
*/
class FEC_API rsc_decoder : virtual public fec::generic_decoder
{
public:
    /*!
    * Build a rsc decoding FEC API object.
    *
    * \param frame_size Number of bits per frame
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
