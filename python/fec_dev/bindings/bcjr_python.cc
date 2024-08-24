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
/* BINDTOOL_HEADER_FILE(bcjr.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(e66ff8b9af93bb9b742df7ee0d027e73)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/fec_dev/bcjr.h>
// pydoc.h is automatically generated in the build directory
#include <bcjr_pydoc.h>

void bind_bcjr(py::module& m)
{


    py::module m_Interleaver = m.def_submodule("Interleaver");


    py::module m_BCJR = m.def_submodule("BCJR");


    py::enum_<::gr::fec_dev::BCJR::_bcjr_impl_t>(m_BCJR, "_bcjr_impl_t")
        .value("STD", ::gr::fec_dev::BCJR::_bcjr_impl_t::STD)             // 0
        .value("FAST", ::gr::fec_dev::BCJR::_bcjr_impl_t::FAST)           // 1
        .value("VERY_FAST", ::gr::fec_dev::BCJR::_bcjr_impl_t::VERY_FAST) // 2
        .value("SCAN", ::gr::fec_dev::BCJR::_bcjr_impl_t::SCAN)           // 3
        .value("GENERIC", ::gr::fec_dev::BCJR::_bcjr_impl_t::GENERIC)     // 4
        .export_values();

    py::implicitly_convertible<int, ::gr::fec_dev::BCJR::_bcjr_impl_t>();


    py::module m_SIMD = m.def_submodule("SIMD");


    py::enum_<::gr::fec_dev::SIMD::_simd_interintra_impl_t>(m_SIMD,
                                                            "_simd_interintra_impl_t")
        .value("X2_AVX", ::gr::fec_dev::SIMD::_simd_interintra_impl_t::X2_AVX) // 0
        .value("X2_SSE", ::gr::fec_dev::SIMD::_simd_interintra_impl_t::X2_SSE) // 1
        .value("X4_AVX", ::gr::fec_dev::SIMD::_simd_interintra_impl_t::X4_AVX) // 2
        .value("NA", ::gr::fec_dev::SIMD::_simd_interintra_impl_t::NA)         // -1
        .export_values();

    py::implicitly_convertible<int, ::gr::fec_dev::SIMD::_simd_interintra_impl_t>();
}