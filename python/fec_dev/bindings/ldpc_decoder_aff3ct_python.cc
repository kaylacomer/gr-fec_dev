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
/* BINDTOOL_HEADER_FILE(ldpc_decoder_aff3ct.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(7f439f356a51e482147f54f792b28012)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/fec_dev/ldpc_decoder_aff3ct.h>
// pydoc.h is automatically generated in the build directory
#include <ldpc_decoder_aff3ct_pydoc.h>

void bind_ldpc_decoder_aff3ct(py::module& m)
{
    using ldpc_decoder_aff3ct = ::gr::fec_dev::ldpc_decoder_aff3ct;


    py::class_<ldpc_decoder_aff3ct, gr::fec::generic_decoder,
        std::shared_ptr<ldpc_decoder_aff3ct>>(m, "ldpc_decoder_aff3ct", D(ldpc_decoder_aff3ct))

        .def_static("make", &ldpc_decoder_aff3ct::make,
           py::arg("K") = 0,
           py::arg("N") = 0,
           py::arg("dec_h_parity_matrix_path") = "",
           py::arg("dec_type") = ::gr::fec_dev::LDPC::_decoder_t::BIT_FLIPPING,
           py::arg("dec_impl") = ::gr::fec_dev::LDPC::_dec_impl_t::STD,
           py::arg("simd_impl") = ::gr::fec_dev::SIMD::_simd_strat_t::SEQ,
           py::arg("dec_h_reorder") = ::gr::fec_dev::LDPC::_dec_H_reorder_t::NONE,
           py::arg("dec_min_AMS") = ::gr::fec_dev::LDPC::_dec_min_AMS_t::MINL,
           py::arg("dec_norm_NMS") = 1.0,
           py::arg("dec_offset_OMS") = 1.0,
           py::arg("dec_mwbf_factor") = 0.0,
           py::arg("dec_synd_depth") = 1,
           py::arg("ppbf_prob") = std::vector<float>{},
           py::arg("dec_no_synd") = false,
           D(ldpc_decoder_aff3ct,make)
        )

        .def("set_frame_size",
            &ldpc_decoder_aff3ct::set_frame_size,
            py::arg("frame_size"),
            D(ldpc_decoder_aff3ct, set_frame_size))

        .def("rate", &ldpc_decoder_aff3ct::rate, D(ldpc_decoder_aff3ct, rate))

        ;
}








