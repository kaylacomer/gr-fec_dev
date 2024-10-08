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
/* BINDTOOL_HEADER_FILE(turbo_decoder.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(b34b63f1161419112ca8a13238c58614)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/fec_dev/turbo_decoder.h>
// pydoc.h is automatically generated in the build directory
#include <turbo_decoder_pydoc.h>

void bind_turbo_decoder(py::module& m)
{
    using turbo_decoder = ::gr::fec_dev::turbo_decoder;

    py::class_<turbo_decoder, gr::fec::generic_decoder, std::shared_ptr<turbo_decoder>>(
        m, "turbo_decoder", D(turbo_decoder))

        // .def(
        //     py::init(&turbo_decoder::make), py::arg("frame_size"), D(turbo_decoder, make))
        
        .def_static("make",
                    &turbo_decoder::make,
                    py::arg("frame_size"),
                    py::arg("n_iterations") = 6,
                    py::arg("standard") = ::gr::fec_dev::Turbo::_enc_standard_t::LTE,
                    py::arg("buffered") = true,
                    py::arg("polys") = std::vector<int>{013, 015},
                    py::arg("trellis_size") = 8,
                    py::arg("quant_fixed_point_pos") = 2,
                    py::arg("quant_saturation_pos") = 6,
                    py::arg("set_sat_pos") = false,
                    py::arg("quant_impl") = ::gr::fec_dev::Quantizer::_quantizer_impl_t::STD,
                    py::arg("dec_impl") = ::gr::fec_dev::Decoder::_decoder_impl_t::STD,
                    py::arg("bcjr_impl") = ::gr::fec_dev::BCJR::_bcjr_impl_t::GENERIC,
                    py::arg("simd_strat") = ::gr::fec_dev::SIMD::_simd_strat_t::SEQ,
                    py::arg("simd_interintra_impl") = ::gr::fec_dev::SIMD::_simd_interintra_impl_t::NA,
                    py::arg("interleaver") = ::gr::fec_dev::Interleaver::_interleaver_t::NO,
                    py::arg("read_order") = ::gr::fec_dev::Interleaver::_itl_read_order_t::NA,
                    py::arg("itl_n_cols") = -1,
                    D(turbo_decoder, make))

        .def("set_frame_size",
             &turbo_decoder::set_frame_size,
             py::arg("frame_size"),
             D(turbo_decoder, set_frame_size))


        .def("rate", &turbo_decoder::rate, D(turbo_decoder, rate))

        ;
}
