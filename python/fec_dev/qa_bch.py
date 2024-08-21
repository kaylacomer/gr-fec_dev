from gnuradio import gr, gr_unittest
from gnuradio import fec
from gnuradio import fec_dev
from _qa_helper import _qa_helper

class test_bch(gr_unittest.TestCase):
    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_basic001(self):
        frame_bits = 248
        data_size = frame_bits//8
        t = 5

        enc = fec_dev.bch_encoder.make(frame_bits, t)
        dec = fec.dummy_decoder.make(frame_bits)
        dec = fec_dev.bch_decoder.make(frame_bits, t)

        threading = None
        self.test = _qa_helper(data_size, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_out = self.test.snk_output.data()
        data_in = self.test.snk_input.data()[0:len(data_out)]

        self.assertEqual(data_in, data_out)
    
    def test_basic002(self):
        frame_bits = 222
        data_size = frame_bits//8
        t = 7

        enc = fec_dev.bch_encoder.make(frame_bits, t)
        dec = fec.dummy_decoder.make(frame_bits)
        dec = fec_dev.bch_decoder.make(frame_bits, t)

        threading = None
        self.test = _qa_helper(data_size, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_out = self.test.snk_output.data()
        data_in = self.test.snk_input.data()[0:len(data_out)]

        self.assertEqual(data_in, data_out)

if __name__ == '__main__':
    gr_unittest.run(test_bch)
