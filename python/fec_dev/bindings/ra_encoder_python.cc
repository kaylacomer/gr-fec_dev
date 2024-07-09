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
/* BINDTOOL_HEADER_FILE(ra_encoder.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(51333878b3125f7f0ed75da2976e9b84)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/fec_dev/ra_encoder.h>
// pydoc.h is automatically generated in the build directory
#include <ra_encoder_pydoc.h>

void bind_ra_encoder(py::module& m)
{
    using ra_encoder = ::gr::fec_dev::ra_encoder;

    py::class_<ra_encoder, gr::fec::generic_encoder,
        std::shared_ptr<ra_encoder>>(m, "ra_encoder", D(ra_encoder))

        .def_static("make", &ra_encoder::make,
           py::arg("frame_size"),
           py::arg("codeword"),
           D(ra_encoder,make)
        )

        .def("set_frame_size",
            &ra_encoder::set_frame_size,
            py::arg("frame_size"),
            D(ra_encoder, set_frame_size))

        .def("rate", &ra_encoder::rate, D(ra_encoder, rate))

        ;
}








