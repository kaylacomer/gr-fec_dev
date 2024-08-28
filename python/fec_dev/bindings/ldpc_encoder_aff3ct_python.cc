/*
 * Copyright 2024 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/***********************************************************************************/
/* This file is automatically generated using bindtool and can be manually edited  */
/* The following lines can be configured to regenerate this file during cmake      */
/* If manual edits are made, the following tags should be modified accordingly.    */
/* BINDTOOL_GEN_AUTOMATIC(0)                                                       */
/* BINDTOOL_USE_PYGCCXML(0)                                                        */
/* BINDTOOL_HEADER_FILE(ldpc_encoder_aff3ct.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(c606875453a6fdcf6972de6c2bd5463b)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/fec_dev/ldpc_encoder_aff3ct.h>
// pydoc.h is automatically generated in the build directory
#include <ldpc_encoder_aff3ct_pydoc.h>

void bind_ldpc_encoder_aff3ct(py::module& m)
{
    using ldpc_encoder_aff3ct = ::gr::fec_dev::ldpc_encoder_aff3ct;


    py::class_<ldpc_encoder_aff3ct, gr::fec::generic_encoder,
        std::shared_ptr<ldpc_encoder_aff3ct>>(m, "ldpc_encoder_aff3ct", D(ldpc_encoder_aff3ct))

        .def_static("make", &ldpc_encoder_aff3ct::make,
           py::arg("K") = 0,
           py::arg("N") = 0,
           py::arg("enc_type") = ::gr::fec_dev::LDPC::_encoder_t::LDPC,
           py::arg("enc_gen_matrix_path") = "",
           py::arg("enc_gen_matrix_method") = ::gr::fec_dev::LDPC::_enc_gen_matrix_method_t::IDENTITY,
           py::arg("enc_gen_matrix_save_path") = "",
           D(ldpc_encoder_aff3ct,make)
        )

        .def("set_frame_size",
            &ldpc_encoder_aff3ct::set_frame_size,
            py::arg("frame_size"),
            D(ldpc_encoder_aff3ct, set_frame_size))

        .def("rate", &ldpc_encoder_aff3ct::rate, D(ldpc_encoder_aff3ct, rate))

        ;
}








