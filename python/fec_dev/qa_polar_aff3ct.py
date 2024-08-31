from gnuradio import gr, gr_unittest
from gnuradio import fec
from gnuradio import fec_dev
from _qa_helper import _qa_helper

class test_polar(gr_unittest.TestCase):
    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_basic001(self):
        frame_bits = 176
        N = 256
        sigma = 0.6692
        data_size = frame_bits//8

        enc = fec_dev.polar_encoder_aff3ct.make(frame_bits, N, sigma)
        dec = fec_dev.polar_decoder_aff3ct.make(frame_bits, N, sigma)

        threading = None
        self.test = _qa_helper(data_size, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_out = self.test.snk_output.data()
        data_in = self.test.snk_input.data()[0:len(data_out)]
        print('data')
        print(data_out)
        print(data_in)

        self.assertEqual(data_in, data_out)
    
    def test_basic002(self):
        frame_bits = 102
        N = 256
        sigma = 0.6692
        data_size = frame_bits//8

        enc = fec_dev.polar_encoder_aff3ct.make(frame_bits, N, sigma)
        dec = fec_dev.polar_decoder_aff3ct.make(frame_bits, N, sigma)

        threading = None
        self.test = _qa_helper(data_size, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_out = self.test.snk_output.data()
        data_in = self.test.snk_input.data()[0:len(data_out)]
        print('data')
        print(data_out)
        print(data_in)

        self.assertEqual(data_in, data_out)
    
if __name__ == '__main__':
    gr_unittest.run(test_polar)
