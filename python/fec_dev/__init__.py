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

ra_encoder.__init__ = ra_encoder.make
ra_encoder_make = ra_encoder.make

ra_decoder.__init__ = ra_decoder.make
ra_decoder_make = ra_decoder.make

rs_encoder.__init__ = rs_encoder.make
rs_encoder_make = rs_encoder.make

rs_decoder.__init__ = rs_decoder.make
rs_decoder_make = rs_decoder.make

rep_encoder.__init__ = rep_encoder.make
rep_encoder_make = rep_encoder.make

rep_decoder.__init__ = rep_decoder.make
rep_decoder_make = rep_decoder.make