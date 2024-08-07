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
/* BINDTOOL_HEADER_FILE(tpc_decoder_aff3ct.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(3f3cd5a196597f6645cdbf0cd744deed)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/fec_dev/tpc_decoder_aff3ct.h>
// pydoc.h is automatically generated in the build directory
#include <tpc_decoder_aff3ct_pydoc.h>

void bind_tpc_decoder_aff3ct(py::module& m)
{
    using tpc_decoder_aff3ct = ::gr::fec_dev::tpc_decoder_aff3ct;


    py::class_<tpc_decoder_aff3ct, gr::fec::generic_decoder,
        std::shared_ptr<tpc_decoder_aff3ct>>(m, "tpc_decoder_aff3ct", D(tpc_decoder_aff3ct))

        .def_static("make", &tpc_decoder_aff3ct::make,
           py::arg("frame_size"),
           D(tpc_decoder_aff3ct,make)
        )

        .def("set_frame_size",
            &tpc_decoder_aff3ct::set_frame_size,
            py::arg("frame_size"),
            D(tpc_decoder_aff3ct, set_frame_size))

        .def("rate", &tpc_decoder_aff3ct::rate, D(tpc_decoder_aff3ct, rate))

        ;
}








