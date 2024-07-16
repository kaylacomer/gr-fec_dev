/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "bch_encoder_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace fec_dev {

fec::generic_encoder::sptr bch_encoder::make(int frame_bits, uint8_t t)
{
    return fec::generic_encoder::sptr(new bch_encoder_impl(frame_bits, t));
}
    /*
     * The private constructor
     */
    bch_encoder_impl::bch_encoder_impl(int frame_bits, uint8_t t)
        : generic_encoder("bch_encoder"),
        d_frame_size(frame_bits),
        d_t(t)
    {
        set_frame_size(frame_bits);

        if (d_K < 3) {
            throw std::runtime_error("K < 3 - frame_bits and t incompatible. Reduce t or increase frame_bits");
        }

        d_poly_gen = std::make_unique<aff3ct::tools::BCH_polynomial_generator<B_8>>(d_N, t);

        d_encoder = std::make_unique<aff3ct::module::Encoder_BCH<B_8>>(d_K, d_N, *d_poly_gen);
}

/*
* Our virtual destructor.
*/
bch_encoder_impl::~bch_encoder_impl()
{
}

int bch_encoder_impl::get_output_size() { return d_N - (d_K - d_frame_size); }
int bch_encoder_impl::get_input_size() { return d_frame_size; }

bool bch_encoder_impl::set_frame_size(unsigned int frame_bits)
{
    bool ret = true;

    uint8_t m = std::ceil(std::log2(frame_bits+1));
    d_N = std::pow(2,m)-1;
    d_K = d_N - m*d_t;

    return ret;
}

double bch_encoder_impl::rate() { return d_frame_size / (d_N - (d_K - d_frame_size)); }

void bch_encoder_impl::generic_work(const void* inbuffer, void* outbuffer)
{
    const B_8* in = (const B_8*)inbuffer;
    std::vector<B_8> input_vector(in, in + d_frame_size);

    int zeros = d_K - d_frame_size;
    if (zeros > 0) {
        input_vector.insert(input_vector.begin(), zeros, 0);
    }

    B_8* out = (B_8*)outbuffer;
    std::vector<B_8> tmp_output(d_N);

    d_encoder->encode(input_vector.data(), tmp_output.data());

    if (zeros > 0) {
        tmp_output.erase(tmp_output.begin(), tmp_output.begin() + zeros);
    }

    std::memcpy(out, tmp_output.data() + zeros, d_N - zeros);
}

} /* namespace fec_dev */
} /* namespace gr */
