from gnuradio import gr, gr_unittest
from gnuradio import fec
from gnuradio import fec_dev
from _qa_helper import _qa_helper

class test_ra(gr_unittest.TestCase):
    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_basic(self):
        frame_size = 40 # in bits
        data_size = frame_size//8
        rep = 3

        enc = fec_dev.rep_encoder.make(frame_size, rep)
        dec = fec_dev.rep_decoder.make(frame_size, rep)

        threading = None
        self.test = _qa_helper(data_size, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_out = self.test.snk_output.data()
        data_in = self.test.snk_input.data()[0:len(data_out)]

        self.assertEqual(data_in, data_out)
    
    def test_basic2(self):
        frame_size = 200 # in bits
        data_size = frame_size//8
        rep = 6

        enc = fec_dev.rep_encoder.make(frame_size, rep)
        dec = fec_dev.rep_decoder.make(frame_size, rep)

        threading = None
        self.test = _qa_helper(data_size, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_out = self.test.snk_output.data()
        data_in = self.test.snk_input.data()[0:len(data_out)]

        self.assertEqual(data_in, data_out)
    
    def test_basic3(self):
        frame_size = 2047 # in bits
        data_size = frame_size//8
        rep = 4

        enc = fec_dev.rep_encoder.make(frame_size, rep)
        dec = fec_dev.rep_decoder.make(frame_size, rep)

        threading = None
        self.test = _qa_helper(data_size, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_out = self.test.snk_output.data()
        data_in = self.test.snk_input.data()[0:len(data_out)]

        self.assertEqual(data_in, data_out)

if __name__ == '__main__':
    gr_unittest.run(test_ra)
