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
/* BINDTOOL_HEADER_FILE(bch_decoder.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(93511c4f47d79f07936089096b400e16)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/fec_dev/bch_decoder.h>
// pydoc.h is automatically generated in the build directory
#include <bch_decoder_pydoc.h>

void bind_bch_decoder(py::module& m)
{
    using bch_decoder = ::gr::fec_dev::bch_decoder;

    py::class_<bch_decoder, gr::fec::generic_decoder,
        std::shared_ptr<bch_decoder>>(m, "bch_decoder", D(bch_decoder))

        .def_static("make", &bch_decoder::make,
           py::arg("frame_bits"),
           py::arg("t") = 5,
           D(bch_decoder,make)
        )

        .def("set_frame_size",
            &bch_decoder::set_frame_size,
            py::arg("frame_bits"),
            D(bch_decoder, set_frame_size))

        .def("rate", &bch_decoder::rate, D(bch_decoder, rate))

        ;
}








