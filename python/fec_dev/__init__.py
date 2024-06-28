#
# Copyright 2008,2009 Free Software Foundation, Inc.
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

# The presence of this file turns this directory into a Python package

'''
This is the GNU Radio FEC_DEV module. Place your Python package
description here (python/__init__.py).
'''
import os

# import pybind11 generated symbols into the fec_dev namespace
try:
    # this might fail if the module is python-only
    from .fec_dev_python import *
except ModuleNotFoundError:
    pass

# Pybind cannot bind constructors to make functions that return a different type
# Remap make functions to __init__ here
turbo_encoder.__init__ = turbo_encoder.make
turbo_encoder_make = turbo_encoder.make

turbo_decoder.__init__ = turbo_decoder.make
turbo_decoder_make = turbo_decoder.make

bch_encoder.__init__ = bch_encoder.make
bch_encoder_make = bch_encoder.make

bch_decoder.__init__ = bch_decoder.make
bch_decoder_make = bch_decoder.make