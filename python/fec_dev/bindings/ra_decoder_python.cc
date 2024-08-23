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
/* BINDTOOL_HEADER_FILE(ra_decoder.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(2973f22b98a06840fadc6e176b0bc003)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/fec_dev/ra_decoder.h>
// pydoc.h is automatically generated in the build directory
#include <ra_decoder_pydoc.h>

void bind_ra_decoder(py::module& m)
{

    using ra_decoder = ::gr::fec_dev::ra_decoder;


    py::class_<ra_decoder, gr::fec::generic_decoder, std::shared_ptr<ra_decoder>>(
        m, "ra_decoder", D(ra_decoder))

        .def_static("make", &ra_decoder::make,
             py::arg("K"),
             py::arg("rep") = 3,
             py::arg("iter") = 1,
             py::arg("interleaver") = ::gr::fec_dev::Interleaver::_interleaver_t::RANDOM,
             py::arg("read_order") = ::gr::fec_dev::Interleaver::_itl_read_order_t::NA,
             py::arg("itl_n_cols") = -1,
             py::arg("quant_fixed_point_pos") = 2,
             py::arg("quant_saturation_pos") = 6,
             py::arg("quant_impl") =::gr::fec_dev::Quantizer::_quantizer_impl_t::STD,
             D(ra_decoder, make))

        .def("set_frame_size",
             &ra_decoder::set_frame_size,
             py::arg("K"),
             D(ra_decoder, set_frame_size))


        .def("rate", &ra_decoder::rate, D(ra_decoder, rate))

        ;
}
