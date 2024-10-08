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
/* BINDTOOL_HEADER_FILE(polar_common_aff3ct.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(7a951a95c78415edd039062ffee4ecc1)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/fec_dev/polar_common_aff3ct.h>
// pydoc.h is automatically generated in the build directory
// #include <polar_common_aff3ct_pydoc.h>

void bind_polar_common_aff3ct(py::module& m)
{


    py::module m_Polar = m.def_submodule("Polar");


    py::enum_<::gr::fec_dev::Polar::_frozen_bit_gen_t>(m_Polar, "_frozen_bit_gen_t")
        .value("GA_ARIKAN", ::gr::fec_dev::Polar::_frozen_bit_gen_t::GA_ARIKAN) // 0
        .value("FIVE_G", ::gr::fec_dev::Polar::_frozen_bit_gen_t::FIVE_G)       // 1
        .value("BEC", ::gr::fec_dev::Polar::_frozen_bit_gen_t::BEC)             // 2
        .value("GA", ::gr::fec_dev::Polar::_frozen_bit_gen_t::GA)               // 3
        .value("TV", ::gr::fec_dev::Polar::_frozen_bit_gen_t::TV)               // 4
        .export_values();

    py::implicitly_convertible<int, ::gr::fec_dev::Polar::_frozen_bit_gen_t>();
    py::enum_<::gr::fec_dev::Polar::_decoder_t>(m_Polar, "_decoder_t")
        .value("SC", ::gr::fec_dev::Polar::_decoder_t::SC)     // 0
        .value("ASCL", ::gr::fec_dev::Polar::_decoder_t::ASCL) // 1
        .value("SCAN", ::gr::fec_dev::Polar::_decoder_t::SCAN) // 2
        .value("SCF", ::gr::fec_dev::Polar::_decoder_t::SCF)   // 3
        .value("SCL", ::gr::fec_dev::Polar::_decoder_t::SCL)   // 4
        .export_values();

    py::implicitly_convertible<int, ::gr::fec_dev::Polar::_decoder_t>();
    py::enum_<::gr::fec_dev::Polar::_noise_t>(m_Polar, "_noise_t")
        .value("Sigma", ::gr::fec_dev::Polar::_noise_t::Sigma) // 0
        .value("Received_optical_power",
               ::gr::fec_dev::Polar::_noise_t::Received_optical_power) // 1
        .value("Event_probability",
               ::gr::fec_dev::Polar::_noise_t::Event_probability) // 2
        .export_values();

    py::implicitly_convertible<int, ::gr::fec_dev::Polar::_noise_t>();
}
