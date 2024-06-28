import numpy as np
from gnuradio import gr, gr_unittest, blocks, fec

class test_fec_dev(gr_unittest.TestCase):
    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001_setup(self):
        block_size = 16
        num_info_bits = 8
