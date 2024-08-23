/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_TURBO_DECODER_H
#define INCLUDED_FEC_DEV_TURBO_DECODER_H

#include <gnuradio/fec/api.h>
#include <gnuradio/fec/generic_decoder.h>
#include "gnuradio/fec_dev/turbo_common.h"
#include "gnuradio/fec_dev/aff3ct_decoder.h"
#include "gnuradio/fec_dev/bcjr.h"
#include <map>
#include <string>

namespace gr {
namespace fec_dev {

/*!
* \brief Turbo Decoding class.
* \ingroup error_coding_blk
*/
class FEC_API turbo_decoder : virtual public fec::generic_decoder
{
public:
    /*!
    * Build a turbo decoding FEC API object.
    *
    * \param frame_bits Number of bits per frame
    * \param n_iterations Maximum number of iterations of decoder
    * \param standard Select a standard (LTE or CCSDS) to
    *        automatically set some parameters
    * \param buffered Buffered encoding. Impacts encoded bit organization
    * \param polys Polynomials that define the two identical RSC sub-encoders.
    *        Set in octal
    * \param trellis_size Number of trellis stages. 8 for LTE, 16 for CCSDS
    * \param quant_impl Quantizer implementation - STD/FAST/NO
    * \param subenc_impl RSC subencoder implementation. Determines whether user
    *        sets polynomials or number of D flip flops
    * \param n_ff Number of D flip flops. Only needed for generic subencoder impl
    * \param dec_impl Decoder implementation: fast or standard
    * \param bcjr_impl BCJR subdecoder implementation: STD, FAST, VERY_FAST, GENERIC
    * \param simd_strat SIMD strategy. Normally SEQ -- sequential. Can be set to INTER or INTRA
    * \param simd_interintra_impl If using INTER_INTRA SIMD strategy, choose implementation
    * * \param interleaver Type of interleaver. Only needs to be set if not using LTE or CCSDS standard
    * \param read_order Interleaver read order for COL_ROW, ROW_COL types
    * \param itl_n_cols Interleaver number of columns for column/row types
    */
    static generic_decoder::sptr make(int frame_bits,
                                      int n_iterations = 6,
                                      Turbo::enc_standard_t standard=Turbo::LTE,
                                      bool buffered=true,
                                      std::vector<int> polys={013,015},
                                      int trellis_size = 8,
                                      Quantizer::quantizer_impl_t quant_impl=Quantizer::STD,
                                      Turbo::subenc_implem_t subenc_impl=Turbo::sys,
                                      int n_ff = -1,
                                      Decoder::decoder_impl_t dec_impl=Decoder::STD,
                                      BCJR::bcjr_impl_t bcjr_impl=BCJR::GENERIC,
                                      SIMD::simd_strat_t simd_strat=SIMD::SEQ,
                                      SIMD::simd_interintra_impl_t simd_interintra_impl=SIMD::NA,
                                      Interleaver::interleaver_t interleaver=Interleaver::NO,
                                      Interleaver::itl_read_order_t read_order=Interleaver::NA,
                                      int itl_n_cols = -1);

    /*!
    * Sets the uncoded frame size to \p frame_bits. If \p
    * frame_bits is greater than the value given to the
    * constructor, the frame size will be capped by that initial
    * value and this function will return false. Otherwise, it
    * returns true.
    */
    bool set_frame_size(unsigned int frame_bits) override = 0;

    /*!
    * Returns the coding rate of this decoder (it will always be 1).
    */
    double rate() override = 0;
};

} // namespace fec_dev
} // namespace gr

#endif /* INCLUDED_FEC_DEV_TURBO_DECODER_H */
