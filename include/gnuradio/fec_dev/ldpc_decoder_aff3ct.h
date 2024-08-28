/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_LDPC_DECODER_AFF3CT_H
#define INCLUDED_FEC_DEV_LDPC_DECODER_AFF3CT_H

#include <gnuradio/fec_dev/api.h>
#include <gnuradio/fec/generic_decoder.h>
#include <gnuradio/fec_dev/aff3ct_decoder.h>
#include <gnuradio/fec_dev/ldpc_common_aff3ct.h>

namespace gr {
namespace fec_dev {

/*!
* \brief ldpc decoding class.
* \ingroup error_coding_blk
*
* \details
* A ldpc decoder class
*/
class FEC_API ldpc_decoder_aff3ct : virtual public fec::generic_decoder
{
public:
    /*!
    * Build a ldpc decoding FEC API object.
    *
    * \param frame_size Number of bits per frame
    */
    static generic_decoder::sptr make(int K = 0,
                           int N = 0,
                           std::string dec_h_parity_matrix_path="",
                           LDPC::decoder_t dec_type = LDPC::BIT_FLIPPING,
                           LDPC::dec_impl_t dec_impl = LDPC::STD,
                           SIMD::simd_strat_t simd_impl = SIMD::SEQ,
                           LDPC::dec_H_reorder_t dec_h_reorder = LDPC::NONE,
                           LDPC::dec_min_AMS_t dec_min_AMS = LDPC::MINL,
                           float dec_norm_NMS = 1.0,
                           float dec_offset_OMS = 1.0,
                           float dec_mwbf_factor = 0.0,
                           int dec_synd_depth = 1,
                           std::vector<float> ppbf_prob={},
                           bool dec_no_synd = false);

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

#endif /* INCLUDED_FEC_DEV_LDPC_DECODER_AFF3CT_H */
