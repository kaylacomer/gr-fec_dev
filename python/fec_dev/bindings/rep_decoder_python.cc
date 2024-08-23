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
/* BINDTOOL_HEADER_FILE(rep_decoder.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(414d790ba74ae4ba85de161bd2e48dad)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/fec_dev/rep_decoder.h>
// pydoc.h is automatically generated in the build directory
#include <rep_decoder_pydoc.h>

void bind_rep_decoder(py::module& m)
{
    using rep_decoder = ::gr::fec_dev::rep_decoder;


    py::class_<rep_decoder, gr::fec::generic_decoder,
        std::shared_ptr<rep_decoder>>(m, "rep_decoder", D(rep_decoder))

        .def_static("make", &rep_decoder::make,
           py::arg("K"),
           py::arg("rep") = 3,
           py::arg("buffered") = true,
           py::arg("quant_fixed_point_pos") = 2,
           py::arg("quant_saturation_pos") = 6,
           py::arg("quant_impl") =::gr::fec_dev::Quantizer::_quantizer_impl_t::STD,
           py::arg("dec_impl") =::gr::fec_dev::Decoder::_decoder_impl_t::STD,
           D(rep_decoder,make)
        )

        .def("set_frame_size",
            &rep_decoder::set_frame_size,
            py::arg("frame_size"),
            D(rep_decoder, set_frame_size))

        .def("rate", &rep_decoder::rate, D(rep_decoder, rate))

        ;
}








