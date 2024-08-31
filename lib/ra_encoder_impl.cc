/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/fec_dev/ra_encoder.h>
#include <gnuradio/io_signature.h>
#include "ra_encoder_impl.h"

namespace gr {
namespace fec_dev {

fec::generic_encoder::sptr ra_encoder::make(int K, int rep, Interleaver::interleaver_t interleaver, Interleaver::itl_read_order_t read_order, int itl_n_cols)
{
    return fec::generic_encoder::sptr(std::make_shared<ra_encoder_impl>(K, rep, interleaver, read_order, itl_n_cols));
}
    ra_encoder_impl::ra_encoder_impl(int K, int rep, Interleaver::interleaver_t interleaver, Interleaver::itl_read_order_t read_order, int itl_n_cols)
        : generic_encoder("ra_encoder"),
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
        d_interleaver = std::make_unique<aff3ct::module::Interleaver<B_8>>(*d_interleaver_core);
        d_encoder = std::make_unique<aff3ct::module::Encoder_RA<B_8>>(d_K, d_N, *d_interleaver);
}

ra_encoder_impl::~ra_encoder_impl()
{
}

int ra_encoder_impl::get_output_size() { return d_N; }
int ra_encoder_impl::get_input_size() { return d_K; }

bool ra_encoder_impl::set_frame_size(unsigned int K)
{
    return true;
}

double ra_encoder_impl::rate() { return static_cast<float>(d_N) / d_K; } // block output:input rate, inverse of codec rate

void ra_encoder_impl::generic_work(const void* inbuffer, void* outbuffer)
{
    const B_8* in = (const B_8*)inbuffer;
    B_8* out = (B_8*)outbuffer;

    d_encoder->encode(in, out);
}

} /* namespace fec_dev */
} /* namespace gr */
