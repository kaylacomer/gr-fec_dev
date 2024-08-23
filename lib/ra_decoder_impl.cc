/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/fec_dev/ra_decoder.h>
#include <gnuradio/io_signature.h>
#include "ra_decoder_impl.h"
#include <volk/volk.h>

namespace gr {
namespace fec_dev {

fec::generic_decoder::sptr ra_decoder::make(int K, int rep, int iter, Interleaver::interleaver_t interleaver,
        Interleaver::itl_read_order_t read_order, int itl_n_cols, uint8_t quant_fixed_point_pos, 
        uint8_t quant_saturation_pos, Quantizer::quantizer_impl_t quant_impl)
{
    return fec::generic_decoder::sptr(new ra_decoder_impl(K, rep, iter, interleaver, read_order, itl_n_cols, quant_fixed_point_pos, quant_saturation_pos, quant_impl));
}
    ra_decoder_impl::ra_decoder_impl(int K, int rep, int iter, Interleaver::interleaver_t interleaver, Interleaver::itl_read_order_t read_order, int itl_n_cols,
        uint8_t quant_fixed_point_pos, uint8_t quant_saturation_pos, Quantizer::quantizer_impl_t quant_impl)
        : generic_decoder("ra_decoder"),
        d_K(K),
        d_N(rep * K)
    {
        set_frame_size(K);

        if (interleaver == Interleaver::COL_ROW || interleaver == Interleaver::ROW_COL) {
            std::string order;
            switch (read_order) {
                case Interleaver::TOP_LEFT:
                    order = "TOP_LEFT";
                    break;
                case Interleaver::TOP_RIGHT:
                    order = "TOP_RIGHT";
                    break;
                case Interleaver::BOTTOM_LEFT:
                    order = "BOTTOM_LEFT";
                    break;
                case Interleaver::BOTTOM_RIGHT:
                    order = "BOTTOM_RIGHT";
                    break;
                default:
                    throw std::runtime_error("Need to specify interleaver read order");
            }
            if (interleaver == Interleaver::COL_ROW) d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_column_row<>>(d_N, itl_n_cols, order);
            else d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_row_column<>>(d_N, itl_n_cols, order);
        }
        else {
            switch(interleaver) {
                case Interleaver::NO:
                    d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_NO<>>(d_N);
                    break;
                case Interleaver::RAND_COL:
                    d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_random_column<>>(d_N, itl_n_cols);
                    break;
                case Interleaver::GOLDEN:
                    d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_golden<>>(d_N);
                    break;
                case Interleaver::DVB_RCS1:
                    d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_ARP_DVB_RCS1<>>(d_N);
                    break;
                case Interleaver::DVB_RCS2:
                    d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_ARP_DVB_RCS2<>>(d_N);
                    break;
                case Interleaver::LTE:
                    d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_LTE<>>(d_N);
                    break;
                case Interleaver::CCSDS:
                    d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_CCSDS<>>(d_N);
                    break;
                case Interleaver::RANDOM:
                default:
                    d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_random<>>(d_N);
                    break;
            }
        }

        // d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_NO<>>(d_N);
        d_pi = std::make_unique<aff3ct::module::Interleaver<Q_8>>(*d_interleaver_core);

        if (quant_impl == Quantizer::STD) {
            d_quant = std::make_unique<aff3ct::module::Quantizer_pow2<float, Q_8>>(d_N, quant_fixed_point_pos);
        }
        else if (quant_impl == Quantizer::FAST) {
            d_quant = std::make_unique<aff3ct::module::Quantizer_pow2_fast<float, Q_8>>(d_N, quant_fixed_point_pos);
        }
        else {
            d_quant = std::make_unique<aff3ct::module::Quantizer_NO<float, Q_8>>(d_N);
        }

        // d_quant = std::make_unique<aff3ct::module::Quantizer_pow2_fast<float, Q_8>>(d_N, 2);
        d_quant_input = std::vector<Q_8>(d_N);
        d_tmp_input = std::vector<float>(d_N);

        d_decoder = std::make_unique<aff3ct::module::Decoder_RA<B_8, Q_8>>(d_K, d_N, *d_pi, iter);
}

ra_decoder_impl::~ra_decoder_impl()
{
}

int ra_decoder_impl::get_output_size() { return d_K; }
int ra_decoder_impl::get_input_size() { return d_N; }

bool ra_decoder_impl::set_frame_size(unsigned int K)
{
    return true;
}

double ra_decoder_impl::rate() { return static_cast<float>(d_K) / d_N; } // decoder rate

void ra_decoder_impl::generic_work(const void* inbuffer, void* outbuffer)
{
    const float* in = (const float*)inbuffer;
    B_8* out = (B_8*)outbuffer;

    volk_32f_s32f_multiply_32f(d_tmp_input.data(), in, -1.0f, d_N);
    d_quant->process(d_tmp_input.data(), d_quant_input.data(), -1);
    
    int status = d_decoder->decode_siho(d_quant_input.data(), out, -1);
    if (status == 1) {
        std::cout << "Decoding FAILURE" << std::endl;
    }
}

} /* namespace fec_dev */
} /* namespace gr */
