/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "turbo_encoder_impl.h"
#include <gnuradio/fec/generic_encoder.h>
#include <aff3ct.hpp>
#include <volk/volk.h>
#include <sstream>

namespace gr {
namespace fec_dev {

fec::generic_encoder::sptr turbo_encoder::make(int frame_size,
                                               enc_standard_t standard,
                                               enc_sub_type_t subencoder,
                                               bool buffered,
                                               std::vector<int> polys,
                                               int trellis_size)
{
    return fec::generic_encoder::sptr(new turbo_encoder_impl(frame_size, standard, subencoder, buffered, polys, trellis_size));
}

    /*
     * The private constructor
     */
    turbo_encoder_impl::turbo_encoder_impl(int frame_size,
                                           enc_standard_t standard,
                                           enc_sub_type_t subencoder,
                                           bool buffered,
                                           std::vector<int> polys,
                                           int trellis_size)
        : generic_encoder("turbo_encoder")
    {
        d_max_frame_size = frame_size;
        d_standard = standard;
        d_subencoder = subencoder;
        d_buffered = buffered;
        d_polys = polys;
        d_trellis_size = trellis_size;
        set_frame_size(frame_size);
}

/*
* Our virtual destructor.
*/
turbo_encoder_impl::~turbo_encoder_impl()
{
}

int turbo_encoder_impl::get_output_size() { return d_output_size; }
int turbo_encoder_impl::get_input_size() { return d_frame_size; }

bool turbo_encoder_impl::set_frame_size(unsigned int frame_size)
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
    d_output_size = 3*d_frame_size + 4*int(std::log2(d_trellis_size));

    return ret;
}

double turbo_encoder_impl::rate() { return d_frame_size / d_output_size; }

void turbo_encoder_impl::generic_work(const void* inbuffer, void* outbuffer)
{
    const signed char* in = (const signed char*)inbuffer;
    signed char* out = (signed char*)outbuffer;

    int N_rsc = 2 * (d_frame_size+std::log2(d_trellis_size));
    auto enco_n = aff3ct::module::Encoder_RSC_generic_sys<B_8>(d_frame_size, N_rsc, d_buffered, d_polys);
    auto enco_i = enco_n;


    aff3ct::tools::Interleaver_core_LTE<> core(d_frame_size);
    aff3ct::module::Interleaver<B_8> pi(core);
    int N_turbo = d_output_size;
    auto encoder = std::unique_ptr<aff3ct::module::Encoder_turbo<B_8>>(new aff3ct::module::Encoder_turbo<B_8>(d_frame_size, N_turbo, enco_n, enco_i, pi));;
    encoder->encode(in, out);
}

} /* namespace fec_dev */
} /* namespace gr */
