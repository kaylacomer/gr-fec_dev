/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/fec_dev/rs_decoder.h>
#include <gnuradio/io_signature.h>
#include "rs_decoder_impl.h"
#include <volk/volk.h>

namespace gr {
namespace fec_dev {

fec::generic_decoder::sptr rs_decoder::make(int frame_size, uint8_t t, uint8_t quant_fixed_point_pos, uint8_t quant_saturation_pos,
                    Quantizer::quantizer_impl_t quant_impl, Decoder::decoder_impl_t dec_impl)
{
    return fec::generic_decoder::sptr(std::make_shared<rs_decoder_impl>(frame_size, t, quant_fixed_point_pos, quant_saturation_pos, quant_impl, dec_impl));
}
    rs_decoder_impl::rs_decoder_impl(int frame_size, uint8_t t, uint8_t quant_fixed_point_pos, uint8_t quant_saturation_pos,
                    Quantizer::quantizer_impl_t quant_impl, Decoder::decoder_impl_t dec_impl)
        : generic_decoder("rs_decoder"),
        d_frame_size(frame_size)
    {
        set_frame_size(frame_size);

        uint8_t m = std::ceil(std::log2(frame_size+1));
        d_N = std::pow(2,m)-1;

        d_poly_gen = std::make_unique<aff3ct::tools::RS_polynomial_generator>(d_N, t);
        d_K = d_N - d_poly_gen->get_n_rdncy();

        if (quant_impl == Quantizer::STD) {
            d_quant = std::make_unique<aff3ct::module::Quantizer_pow2<float, Q_8>>(d_codeword_size, quant_fixed_point_pos, quant_saturation_pos);
        }
        else if (quant_impl == Quantizer::FAST) {
            d_quant = std::make_unique<aff3ct::module::Quantizer_pow2_fast<float, Q_8>>(d_codeword_size, quant_fixed_point_pos, quant_saturation_pos);
        }
        else {
            d_quant = std::make_unique<aff3ct::module::Quantizer_NO<float, Q_8>>(d_codeword_size);
        }
        d_quant_input = std::vector<Q_8>(d_N);
        d_tmp_input = std::vector<float>(d_codeword_size);
        d_tmp_output = std::vector<B_8>(d_K);

        if (dec_impl == Decoder::STD) {
            d_decoder = std::make_unique<aff3ct::module::Decoder_RS_std<B_8, Q_8>>(d_K, d_N, *d_poly_gen);
        }
        else if (dec_impl == Decoder::GENIUS) {
            auto encoder = std::make_unique<aff3ct::module::Encoder_RS<B_8>>(d_K, d_N, *d_poly_gen);
            d_decoder = std::make_unique<aff3ct::module::Decoder_RS_genius<B_8, Q_8>>(d_K, d_N, *d_poly_gen, *encoder);
        }
        else {
            throw std::runtime_error("RS decoder has STD and GENIUS implementations");
        }
}

rs_decoder_impl::~rs_decoder_impl()
{
}

int rs_decoder_impl::get_output_size() { return d_frame_size; }
int rs_decoder_impl::get_input_size() { return d_codeword_size; }

bool rs_decoder_impl::set_frame_size(unsigned int frame_size)
{
    return true;
}

double rs_decoder_impl::rate() { return static_cast<float>(d_frame_size) / d_codeword_size; } // decoder rate

void rs_decoder_impl::generic_work(const void* inbuffer, void* outbuffer)
{
    const float* in = (const float*)inbuffer;
    B_8* out = (B_8*)outbuffer;

    volk_32f_s32f_multiply_32f(d_tmp_input.data(), in, -1.0f, d_codeword_size);
    d_quant->process(d_tmp_input.data(), &d_quant_input[d_zeros], -1);
    
    int status = d_decoder->decode_siho(d_quant_input.data(), d_tmp_output.data(), -1);

    if (status == spu::runtime::status_t::SUCCESS) {
        std::memcpy(out, &d_tmp_output[d_zeros], d_frame_size * sizeof(B_8));
    }
    else {
        d_logger->info("Decoding failed");
        std::fill(out, out + d_frame_size * sizeof(B_8), 0);
    }
}

} /* namespace fec_dev */
} /* namespace gr */
