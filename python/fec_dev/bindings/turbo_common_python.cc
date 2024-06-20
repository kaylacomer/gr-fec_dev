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
/* BINDTOOL_GEN_AUTOMATIC(1)                                                       */
/* BINDTOOL_USE_PYGCCXML(0)                                                        */
/* BINDTOOL_HEADER_FILE(turbo_common.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(78039ce69570e6dd1b4715f50b916c08)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/fec_dev/turbo_common.h>
// pydoc.h is automatically generated in the build directory
#include <turbo_common_pydoc.h>

void bind_turbo_common(py::module& m)
{
    py::enum_<::gr::fec_dev::_enc_standard_t>(m, "_enc_standard_t")
        .value("LTE", ::gr::fec_dev::_enc_standard_t::LTE)       // 0
        .value("CCSDS", ::gr::fec_dev::_enc_standard_t::CCSDS)   // 1
        .value("CUSTOM", ::gr::fec_dev::_enc_standard_t::CUSTOM) // 2
        .export_values();

    py::implicitly_convertible<int, ::gr::fec_dev::_enc_standard_t>();

    
    py::enum_<::gr::fec_dev::_enc_sub_type_t>(m, "_enc_sub_type_t")
        .value("RSC", ::gr::fec_dev::_enc_sub_type_t::RSC) // 0
        .export_values();

    py::implicitly_convertible<int, ::gr::fec_dev::_enc_sub_type_t>();
}