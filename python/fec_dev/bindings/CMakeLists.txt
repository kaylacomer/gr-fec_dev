# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Check if there is C++ code at all
########################################################################
if(NOT fec_dev_sources)
    message(STATUS "No C++ sources... skipping python bindings")
    return()
endif(NOT fec_dev_sources)

########################################################################
# Check for pygccxml
########################################################################
gr_python_check_module_raw("pygccxml" "import pygccxml" PYGCCXML_FOUND)

include(GrPybind)

########################################################################
# Python Bindings
########################################################################
list(APPEND fec_dev_python_files
    turbo_common_python.cc
    turbo_encoder_python.cc
    turbo_decoder_python.cc
    bch_encoder_python.cc
    bch_decoder_python.cc
    ra_encoder_python.cc
    ra_decoder_python.cc python_bindings.cc)

gr_pybind_make_oot(fec_dev ../../.. gr::fec_dev "${fec_dev_python_files}")

# copy bindings extension for use in QA test module
add_custom_command(
    TARGET fec_dev_python
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:fec_dev_python>
            ${PROJECT_BINARY_DIR}/test_modules/gnuradio/fec_dev/)

install(
    TARGETS fec_dev_python
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/fec_dev
    COMPONENT pythonapi)
