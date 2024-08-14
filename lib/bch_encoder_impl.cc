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

        uint8_t m = std::ceil(std::log2(frame_bits+1));
        d_N = (1 << m) - 1;
        d_poly_gen = std::make_unique<aff3ct::tools::BCH_polynomial_generator<B_8>>(d_N, d_t);
        d_K = d_N - d_poly_gen->get_n_rdncy();
        d_zeros = d_K - d_frame_size;
        d_codeword_size = d_N - d_zeros;

        if (d_K < 3) {
            throw std::runtime_error("K < 3 - frame_bits and t incompatible. Reduce t or increase frame_bits");
        }

        d_tmp_input = std::vector<B_8>(d_K);
        d_tmp_output = std::vector<B_8>(d_N);

        // d_poly_gen = std::make_unique<aff3ct::tools::BCH_polynomial_generator<B_8>>(d_N, d_t);

        d_encoder = std::make_unique<aff3ct::module::Encoder_BCH<B_8>>(d_K, d_N, *d_poly_gen);
}

/*
* Our virtual destructor.
*/
bch_encoder_impl::~bch_encoder_impl()
{
}

int bch_encoder_impl::get_output_size() { return d_codeword_size; }
int bch_encoder_impl::get_input_size() { return d_frame_size; }

bool bch_encoder_impl::set_frame_size(unsigned int frame_bits)
{
    bool ret = true;

    return ret;
}

double bch_encoder_impl::rate() { return static_cast<float>(d_frame_size) / d_codeword_size; }

void bch_encoder_impl::generic_work(const void* inbuffer, void* outbuffer)
{
    const B_8* in = (const B_8*)inbuffer;
    B_8* out = (B_8*)outbuffer;
    
    std::memcpy(&d_tmp_input[d_zeros], in, d_frame_size * sizeof(B_8));

    d_encoder->encode(d_tmp_input.data(), d_tmp_output.data());

    std::memcpy(out, &d_tmp_output[d_zeros], d_codeword_size * sizeof(B_8));
}

} /* namespace fec_dev */
} /* namespace gr */
