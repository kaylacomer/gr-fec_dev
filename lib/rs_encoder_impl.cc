/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/io_signature.h>
#include "rs_encoder_impl.h"

namespace gr {
namespace fec_dev {

fec::generic_encoder::sptr rs_encoder::make(int frame_size, uint8_t t)
{
    return fec::generic_encoder::sptr(std::make_shared<rs_encoder_impl>(frame_size, t));
}
    rs_encoder_impl::rs_encoder_impl(int frame_size, uint8_t t)
        : generic_encoder("rs_encoder"),
        d_frame_size(frame_size)
    {
        set_frame_size(frame_size);

        uint8_t m = std::ceil(std::log2(frame_size+1));
        d_N = std::pow(2,m)-1;

        d_poly_gen = std::make_unique<aff3ct::tools::RS_polynomial_generator>(d_N, t);
        d_K = d_N - d_poly_gen->get_n_rdncy();

        d_zeros = d_K - d_frame_size;
        d_codeword_size = d_N - d_zeros;
        if (d_zeros) {
            d_logger->info("Padding {:d}-bit input (K_info) with {:d} zeros. K: {:d}, N: {:d}, N_cw: {:d}", d_frame_size, t, d_K, d_N, d_codeword_size);
        }
        if (d_K < 3) {
            throw std::runtime_error("K < 3 - frame_bits and t incompatible. Reduce t or increase frame_bits");
        }

        d_encoder = std::make_unique<aff3ct::module::Encoder_RS<B_8>>(d_K, d_N, *d_poly_gen);
}

rs_encoder_impl::~rs_encoder_impl()
{
}

int rs_encoder_impl::get_output_size() { return d_codeword_size; }
int rs_encoder_impl::get_input_size() { return d_frame_size; }

bool rs_encoder_impl::set_frame_size(unsigned int frame_size)
{
    return true;
}

double rs_encoder_impl::rate() { return static_cast<float>(d_frame_size) / d_codeword_size; } // encoder rate

void rs_encoder_impl::generic_work(const void* inbuffer, void* outbuffer)
{
    const B_8* in = (const B_8*)inbuffer;
    B_8* out = (B_8*)outbuffer;
    
    std::memcpy(&d_tmp_input[d_zeros], in, d_frame_size * sizeof(B_8));

    d_encoder->encode(d_tmp_input.data(), d_tmp_output.data());

    std::memcpy(out, &d_tmp_output[d_zeros], d_codeword_size * sizeof(B_8));
}

} /* namespace fec_dev */
} /* namespace gr */
