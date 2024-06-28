from gnuradio import gr, gr_unittest
from gnuradio import fec
from gnuradio import fec_dev
from _qa_helper import _qa_helper

class test_turbo(gr_unittest.TestCase):
    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_basic(self):
        frame_size = 40 # in bits
        data_size = frame_size//8

        enc = fec_dev.turbo_encoder.make(frame_size)
        dec = fec_dev.turbo_decoder.make(frame_size)

        threading = None
        self.test = _qa_helper(data_size, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_out = self.test.snk_output.data()
        data_in = self.test.snk_input.data()[0:len(data_out)]

        self.assertEqual(data_in, data_out)

if __name__ == '__main__':
    gr_unittest.run(test_turbo)
