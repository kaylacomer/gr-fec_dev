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
    
    def test_LTE(self):
        data_size = 264
        frame_bits = data_size*8

        enc = fec_dev.turbo_encoder.make(frame_bits, fec_dev.LTE)
        dec = fec_dev.turbo_decoder.make(frame_bits, fec_dev.LTE)

        threading = None
        self.test = _qa_helper(data_size, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_out = self.test.snk_output.data()
        data_in = self.test.snk_input.data()[0:len(data_out)]

        self.assertEqual(data_in, data_out)
    
    def test_CCSDS(self):
        frame_bits = 1784
        data_size = frame_bits//8

        enc = fec_dev.turbo_encoder.make(frame_bits, fec_dev.CCSDS)
        dec = fec_dev.turbo_decoder.make(frame_bits, fec_dev.CCSDS)

        threading = None
        self.test = _qa_helper(data_size, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_out = self.test.snk_output.data()
        data_in = self.test.snk_input.data()[0:len(data_out)]

        self.assertEqual(data_in, data_out)
    
    # def test_CUSTOM(self):
    #     data_size = 1784
    #     frame_bits = data_size*8

    #     enc = fec_dev.turbo_encoder.make(frame_bits, fec_dev.CUSTOM)
    #     dec = fec_dev.turbo_decoder.make(frame_bits, fec_dev.CUSTOM)

    #     threading = None
    #     self.test = _qa_helper(data_size, enc, dec, threading)
    #     self.tb.connect(self.test)
    #     self.tb.run()

    #     data_out = self.test.snk_output.data()
    #     data_in = self.test.snk_input.data()[0:len(data_out)]

    #     self.assertEqual(data_in, data_out)

if __name__ == '__main__':
    gr_unittest.run(test_turbo)
