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
/* BINDTOOL_HEADER_FILE(aff3ct_decoder.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(69a29dfb351d3211318ca561d7dc7b15)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/fec_dev/aff3ct_decoder.h>
// pydoc.h is automatically generated in the build directory
#include <aff3ct_decoder_pydoc.h>

void bind_aff3ct_decoder(py::module& m)
{


    py::module m_Quantizer = m.def_submodule("Quantizer");


    py::enum_<::gr::fec_dev::Quantizer::_quantizer_impl_t>(m_Quantizer,
                                                           "_quantizer_impl_t")
        .value("STD", ::gr::fec_dev::Quantizer::_quantizer_impl_t::STD)   // 0
        .value("FAST", ::gr::fec_dev::Quantizer::_quantizer_impl_t::FAST) // 1
        .value("NO", ::gr::fec_dev::Quantizer::_quantizer_impl_t::NO)     // 2
        .export_values();

    py::implicitly_convertible<int, ::gr::fec_dev::Quantizer::_quantizer_impl_t>();


    py::module m_Decoder = m.def_submodule("Decoder");


    py::enum_<::gr::fec_dev::Decoder::_decoder_impl_t>(m_Decoder, "_decoder_impl_t")
        .value("STD", ::gr::fec_dev::Decoder::_decoder_impl_t::STD)             // 0
        .value("FAST", ::gr::fec_dev::Decoder::_decoder_impl_t::FAST)           // 1
        .value("GENIUS", ::gr::fec_dev::Decoder::_decoder_impl_t::GENIUS)       // 2
        .value("VERY_FAST", ::gr::fec_dev::Decoder::_decoder_impl_t::VERY_FAST) // 3
        .export_values();

    py::implicitly_convertible<int, ::gr::fec_dev::Decoder::_decoder_impl_t>();


    py::module m_SIMD = m.def_submodule("SIMD");


    py::enum_<::gr::fec_dev::SIMD::_simd_strat_t>(m_SIMD, "_simd_strat_t")
        .value("SEQ", ::gr::fec_dev::SIMD::_simd_strat_t::SEQ)                 // 0
        .value("INTER", ::gr::fec_dev::SIMD::_simd_strat_t::INTER)             // 1
        .value("INTRA", ::gr::fec_dev::SIMD::_simd_strat_t::INTRA)             // 2
        .value("INTER_INTRA", ::gr::fec_dev::SIMD::_simd_strat_t::INTER_INTRA) // 3
        .export_values();

    py::implicitly_convertible<int, ::gr::fec_dev::SIMD::_simd_strat_t>();
}