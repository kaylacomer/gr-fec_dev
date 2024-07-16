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

fec::generic_encoder::sptr rs_encoder::make(int frame_size)
{
    return fec::generic_encoder::sptr(new rs_encoder_impl(frame_size));
}
    rs_encoder_impl::rs_encoder_impl(int frame_size)
        : generic_encoder("rs_encoder"),
        d_frame_size(frame_size)
    {
        int t=5;
        set_frame_size(frame_size);

        d_poly_gen = std::make_unique<aff3ct::tools::RS_polynomial_generator>(d_N, t);
        d_K = d_N - d_poly_gen->get_n_rdncy();
        if (d_K < 3) {
            throw std::runtime_error("K < 3 - frame_bits and t incompatible. Reduce t or increase frame_bits");
        }

        d_encoder = std::make_unique<aff3ct::module::Encoder_RS<B_8>>(d_K, d_N, *d_poly_gen);
}

rs_encoder_impl::~rs_encoder_impl()
{
}

int rs_encoder_impl::get_output_size() { return d_N; }
int rs_encoder_impl::get_input_size() { return d_frame_size; }

bool rs_encoder_impl::set_frame_size(unsigned int frame_size)
{
    bool ret = true;

    uint8_t m = std::ceil(std::log2(frame_size+1));
    d_N = std::pow(2,m)-1;

    return ret;
}

double rs_encoder_impl::rate() { return d_N - (d_K - d_frame_size) / d_frame_size; } // encoder rate

void rs_encoder_impl::generic_work(const void* inbuffer, void* outbuffer)
{
    const B_8* in = (const B_8*)inbuffer;
    std::vector<B_8> input_vector(in, in + d_frame_size);

    std::cout << "here1" << std::endl;

    int zeros = d_K - d_frame_size;
    if (zeros > 0) {
        input_vector.insert(input_vector.begin(), zeros, 0);
    }

    std::cout << "here2" << std::endl;

    B_8* out = (B_8*)outbuffer;
    std::vector<B_8> tmp_output(d_N);

    std::cout << "here3" << std::endl;

    d_encoder->encode(input_vector.data(), tmp_output.data());

    std::cout << "here4" << std::endl;

    if (zeros > 0) {
        tmp_output.erase(tmp_output.begin(), tmp_output.begin() + zeros);
    }
    std::cout << "here5" << std::endl;

    std::memcpy(out, tmp_output.data() + zeros, d_N - zeros);
    std::cout << "here6" << std::endl;
}

} /* namespace fec_dev */
} /* namespace gr */
