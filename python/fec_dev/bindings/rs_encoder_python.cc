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
/* BINDTOOL_HEADER_FILE(rs_encoder.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(15df46c8700fd8881de08060bde54d7c)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/fec_dev/rs_encoder.h>
// pydoc.h is automatically generated in the build directory
#include <rs_encoder_pydoc.h>

void bind_rs_encoder(py::module& m)
{
    using rs_encoder = ::gr::fec_dev::rs_encoder;


    py::class_<rs_encoder, gr::fec::generic_encoder,
        std::shared_ptr<rs_encoder>>(m, "rs_encoder", D(rs_encoder))

        .def_static("make", &rs_encoder::make,
           py::arg("frame_size"),
           py::arg("t") = 5,
           D(rs_encoder,make)
        )

        .def("set_frame_size",
            &rs_encoder::set_frame_size,
            py::arg("frame_size"),
            D(rs_encoder, set_frame_size))

        .def("rate", &rs_encoder::rate, D(rs_encoder, rate))

        ;
}