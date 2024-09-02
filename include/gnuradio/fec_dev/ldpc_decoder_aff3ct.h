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
* \brief LDPC Decoding class (via AFF3CT library). NOT IMPLEMENTED
* \ingroup error_coding_blk
*
* \details
* This class performs Low-Density Parity-Check (LDPC)
* decoding using the AFF3CT library API. For more information about the decoder parameters, see
* https://aff3ct.readthedocs.io/en/latest/user/simulation/parameters/codec/ldpc/codec.html.
* For more information about the quantization parameters, see
* https://aff3ct.readthedocs.io/en/latest/user/simulation/parameters/quantizer/quantizer.html.
*
* Note: This class is NOT currently functional and needs work. Also, compared to the other AFF3CT-based
* decoders, this class is missing Quantizer parameters (quant_fixed_point_pos, quant_saturation_pos, quant_impl).
* Also missing --dec-ite, number of iterations in the LDPC decoder.
*/
class FEC_API ldpc_decoder_aff3ct : virtual public fec::generic_decoder
{
public:
    /*!
    * Build LDPC decoding FEC API object. Most parameter descriptions are copied from the AFF3CT
    * User Manual directly and currently have no meaning. Class is non-functional
    *
    * \param K Number of bits per frame output from decoder
    * \param N Number of bits per frame input to the decoder
    * \param dec_h_parity_matrix_path Path to the H parity matrix. Supports AList and QC formats.
    * \param dec_type Decoder algorithm
    * \param dec_impl Implementation of decoder algorithm
    * \param simd_impl Single Instruction Multiple Data (SIMD) implementation
    * \param dec_h_reorder Specify the order of execution of the CNs in the decoding process depending on their degree.
    * \param dec_min_AMS Define the min operator approximation used in the AMS update rule.
    * \param dec_norm_NMS Set the normalization factor used in the NMS update rule.
    * \param dec_offset_OMS Set the offset used in the OMS update rule.
    * \param dec_mwbf_factor Give the weighting factor used in the MWBF algorithm.
    * \param dec_synd_depth Set the number of iterations to process before enabling the syndrome detection. In some cases, it can help to avoid false positive detections.
    * \param ppbf_prob Give the probabilities of the Bernouilli distribution of the PPBF. The number of given values must be equal to the biggest variable node degree plus two.
    * \param dec_no_synd Disable the syndrome detection, all the LDPC decoding iterations will be performed.
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
