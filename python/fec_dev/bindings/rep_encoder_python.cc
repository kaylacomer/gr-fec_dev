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
/* BINDTOOL_HEADER_FILE(rep_encoder.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(52ec1b7a669d905884191dcbd44e84b5)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/fec_dev/rep_encoder.h>
// pydoc.h is automatically generated in the build directory
#include <rep_encoder_pydoc.h>

void bind_rep_encoder(py::module& m)
{
    using rep_encoder = ::gr::fec_dev::rep_encoder;


    py::class_<rep_encoder, gr::fec::generic_encoder,
        std::shared_ptr<rep_encoder>>(m, "rep_encoder", D(rep_encoder))

        .def_static("make", &rep_encoder::make,
           py::arg("K"),
           py::arg("rep") = 3,
           py::arg("buffered") = true,
           D(rep_encoder,make)
        )

        .def("set_frame_size",
            &rep_encoder::set_frame_size,
            py::arg("frame_size"),
            D(rep_encoder, set_frame_size))

        .def("rate", &rep_encoder::rate, D(rep_encoder, rate))

        ;
}








