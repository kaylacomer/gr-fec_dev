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

fec::generic_encoder::sptr bch_encoder::make(int codeword, uint8_t t)
{
    return fec::generic_encoder::sptr(new bch_encoder_impl(codeword, t));
}
    /*
     * The private constructor
     */
    bch_encoder_impl::bch_encoder_impl(int codeword, uint8_t t)
        : generic_encoder("bch_encoder"),
        d_max_frame_size(codeword),
        d_t(t)
    {
        set_frame_size(codeword);

        if (d_frame_size < 3) {
            ; //error
        }

        d_poly_gen = std::make_unique<aff3ct::tools::BCH_polynomial_generator<B_8>>(d_output_size, t);
        d_zeros = d_output_size - codeword;

        d_encoder = std::make_unique<aff3ct::module::Encoder_BCH<B_8>>(d_frame_size, d_output_size, *d_poly_gen);
}

/*
* Our virtual destructor.
*/
bch_encoder_impl::~bch_encoder_impl()
{
}

int bch_encoder_impl::get_output_size() { return d_output_size; }
int bch_encoder_impl::get_input_size() { return d_input_size; }

bool bch_encoder_impl::set_frame_size(unsigned int codeword)
{
    bool ret = true;
    if (codeword > d_max_frame_size) {
        d_logger->info("tried to set frame to {:d}; max possible is {:d}",
                    codeword,
                    d_max_frame_size);
        codeword = d_max_frame_size;
        ret = false;
    }

    uint8_t m = std::ceil(std::log2(codeword+1));
    d_output_size = std::pow(2,m)-1;
    d_frame_size = d_input_size - m*d_t;
    d_input_size = d_frame_size;

    return ret;
}

double bch_encoder_impl::rate() { return d_input_size / d_output_size; }

void bch_encoder_impl::generic_work(const void* inbuffer, void* outbuffer)
{
    const B_8* in = (const B_8*)inbuffer;
    B_8* out = (B_8*)outbuffer;

    if (d_zeros > 0) {
        ; //pad input with zeros
    }

    d_encoder->encode(in, out);
}

} /* namespace fec_dev */
} /* namespace gr */
