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
        frame_bits = 176
        data_size = frame_bits//8
        t = 5

        enc = fec_dev.bch_encoder.make(frame_bits, t)
        dec = fec_dev.bch_decoder.make(frame_bits, t)

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
        frame_bits = 222
        data_size = frame_bits//8
        t = 7

        enc = fec_dev.bch_encoder.make(frame_bits, t)
        dec = fec_dev.bch_decoder.make(frame_bits, t)

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
    
    # def test_basic003(self):
    #     frame_bits = 828
    #     data_size = frame_bits//8
    #     t = 20

    #     enc = fec_dev.bch_encoder.make(frame_bits, t)
    #     dec = fec_dev.bch_decoder.make(frame_bits, t)

    #     threading = None
    #     self.test = _qa_helper(data_size, enc, dec, threading)
    #     self.tb.connect(self.test)
    #     self.tb.run()

    #     data_out = self.test.snk_output.data()
    #     data_in = self.test.snk_input.data()[0:len(data_out)]

    #     self.assertEqual(data_in, data_out)
    
    # def test_basic004(self):
    #     frame_bits = 2167
    #     data_size = frame_bits//8
    #     t = 50

    #     enc = fec_dev.bch_encoder.make(frame_bits, t)
    #     dec = fec_dev.bch_decoder.make(frame_bits, t)

    #     threading = None
    #     self.test = _qa_helper(data_size, enc, dec, threading)
    #     self.tb.connect(self.test)
    #     self.tb.run()

    #     data_out = self.test.snk_output.data()
    #     data_in = self.test.snk_input.data()[0:len(data_out)]

    #     self.assertEqual(data_in, data_out)

if __name__ == '__main__':
    gr_unittest.run(test_bch)
