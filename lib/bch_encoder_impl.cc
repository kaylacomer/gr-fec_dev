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

fec::generic_encoder::sptr bch_encoder::make(int frame_size)
{
    return fec::generic_encoder::sptr(new bch_encoder_impl(frame_size));
}
    /*
     * The private constructor
     */
    bch_encoder_impl::bch_encoder_impl(int frame_size)
        : generic_encoder("bch_encoder"),
        d_max_frame_size(frame_size)
    {
        set_frame_size(frame_size);

        int N = 63;
        int t = 3;
        d_poly_gen = std::make_unique<aff3ct::tools::BCH_polynomial_generator<B_8>>(N, t);
        int n_redundancy = d_poly_gen->get_n_rdncy();
        int K = N - n_redundancy;

        d_input_size = K;
        d_output_size = N;

        d_encoder = std::make_unique<aff3ct::module::Encoder_BCH<B_8>>(K, N, *d_poly_gen);
}

/*
* Our virtual destructor.
*/
bch_encoder_impl::~bch_encoder_impl()
{
}

int bch_encoder_impl::get_output_size() { return d_output_size; }
int bch_encoder_impl::get_input_size() { return d_input_size; }

bool bch_encoder_impl::set_frame_size(unsigned int frame_size)
{
    bool ret = true;
    if (frame_size > d_max_frame_size) {
        d_logger->info("tried to set frame to {:d}; max possible is {:d}",
                    frame_size,
                    d_max_frame_size);
        frame_size = d_max_frame_size;
        ret = false;
    }

    d_frame_size = frame_size;

    return ret;
}

double bch_encoder_impl::rate() { return d_input_size / d_output_size; }

void bch_encoder_impl::generic_work(const void* inbuffer, void* outbuffer)
{
    const B_8* in = (const B_8*)inbuffer;
    B_8* out = (B_8*)outbuffer;

    d_encoder->encode(in, out);
}

} /* namespace fec_dev */
} /* namespace gr */
