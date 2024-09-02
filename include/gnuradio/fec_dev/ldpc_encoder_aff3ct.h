/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_LDPC_ENCODER_AFF3CT_H
#define INCLUDED_FEC_DEV_LDPC_ENCODER_AFF3CT_H

#include <gnuradio/fec_dev/api.h>
#include <gnuradio/fec/generic_encoder.h>
#include <gnuradio/fec_dev/ldpc_common_aff3ct.h>

namespace gr {
namespace fec_dev {

/*!
* \brief LDPC Encoding class (via AFF3CT library). NOT IMPLEMENTED
* \ingroup error_coding_blk
*
* \details
* This class performs Low-Density Parity-Check (LDPC)
* encoding using the AFF3CT library API. For more information about the encoder parameters, see
* https://aff3ct.readthedocs.io/en/latest/user/simulation/parameters/codec/ldpc/codec.html.
*
* Note: This class is NOT currently functional and needs work
*/
class FEC_API ldpc_encoder_aff3ct : virtual public fec::generic_encoder
{
public:
    /*!
    * Build LDPC encoding FEC API object. Most parameter descriptions are copied from the AFF3CT
    * User Manual directly and currently have no meaning. Class is non-functional
    *
    * \param K Number of bits per frame output from encoder
    * \param N Number of bits per frame input to the encoder
    * \param enc_type Encoder type
    * \param enc_gen_matrix_path Give the path to the G generator matrix in an AList or QC formated file.
    * \param enc_gen_matrix_method Specify the method used to build the G generator matrix from the H parity matrix when using the LDPC_H encoder.
    * \param enc_gen_matrix_save_path Set the file path where the G generator matrix will be saved (AList file format). To use with the LDPC_H encoder.
    */
    static generic_encoder::sptr make(int K = 0,
                           int N = 0,
                           LDPC::encoder_t enc_type = LDPC::LDPC,
                           std::string enc_gen_matrix_path="",
                           LDPC::enc_gen_matrix_method_t enc_gen_matrix_method = LDPC::IDENTITY,
                           std::string enc_gen_matrix_save_path="");

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

#endif /* INCLUDED_FEC_DEV_LDPC_ENCODER_AFF3CT_H */
