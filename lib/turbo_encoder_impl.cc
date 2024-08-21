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

namespace gr {
namespace fec_dev {

fec::generic_encoder::sptr turbo_encoder::make(int frame_bits,
                                      Turbo::enc_standard_t standard,
                                      bool buffered,
                                      std::vector<int> polys,
                                      int trellis_size,
                                      Turbo::subenc_implem_t subenc_impl,
                                      int n_ff,
                                      Interleaver::itl_read_order_t read_order,
                                      int itl_n_cols)
{
    return fec::generic_encoder::sptr(new turbo_encoder_impl(
        frame_bits, standard, buffered, polys, trellis_size, subenc_impl,
        n_ff, read_order, itl_n_cols));
}

/*
 * The private constructor
 */
turbo_encoder_impl::turbo_encoder_impl(int frame_bits,
                                      Turbo::enc_standard_t standard,
                                      bool buffered,
                                      std::vector<int> polys,
                                      int trellis_size,
                                      Turbo::subenc_implem_t subenc_impl,
                                      int n_ff,
                                      Interleaver::itl_read_order_t read_order,
                                      int itl_n_cols)
    : generic_encoder("turbo_encoder"),
      d_K(frame_bits),
      d_trellis_size(trellis_size)
{
    if (standard == Turbo::LTE) {
        d_trellis_size = 8;
        polys = {013,015};
        d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_LTE<>>(d_K);
    }
    else if (standard == Turbo::CCSDS) {
        std::vector<int> CCSDS_sizes = {1784, 3568, 7136, 8920};
        if (!count(CCSDS_sizes.begin(), CCSDS_sizes.end(), d_K)) {
            throw std::runtime_error("CCSDS supported frame sizes are 1784, 3568, 7136, 8920");
        }

        d_trellis_size = 16;
        polys = {023,033};
        d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_CCSDS<>>(d_K);
    }
    // else if (standard == NO) {
    //     d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_NO<>>(d_K);
    // }
    // // else if (standard == COL_ROW) {
    // //     const int n_cols = 1;
    // //     itl_read_order_t read_order = TOP_LEFT;
    // //     d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_column_row<>>(d_K, n_cols, read_order);
    // // }
    // // else if (standard == ROW_COL) {
    // //     const int n_cols = 1;
    // //     itl_read_order_t read_order = TOP_LEFT;
    // //     d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_row_column<>>(d_K, n_cols, read_order);
    // // }
    // else if (standard == RAND_COL) {
    //     int n_cols = 1;
    //     d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_random_column<>>(d_K, n_cols);
    // }
    // else if (standard == GOLDEN) {
    //     d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_golden<>>(d_K);
    // }
    // else if (standard == DVB_RCS1) {
    //     d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_ARP_DVB_RCS1<>>(d_K);
    // }
    // else if (standard == DVB_RCS2) {
    //     d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_ARP_DVB_RCS2<>>(d_K);
    // }
    else {
        d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_random<>>(d_K);
    }

    d_N = 3 * d_K + 4 * int(std::log2(d_trellis_size));

    int N_rsc = 2 * (d_K + std::log2(d_trellis_size));
    auto enco_n = aff3ct::module::Encoder_RSC_generic_sys<B_8>(d_K, N_rsc, buffered, polys);
    auto enco_i = enco_n;
    
    d_pi = std::make_unique<aff3ct::module::Interleaver<B_8>>(*d_interleaver_core);
    d_encoder = std::make_unique<aff3ct::module::Encoder_turbo<B_8>>(d_K, d_N, enco_n, enco_i, *d_pi);
}

/*
 * Our virtual destructor.
 */
turbo_encoder_impl::~turbo_encoder_impl() {}

int turbo_encoder_impl::get_output_size() { return d_N; }
int turbo_encoder_impl::get_input_size() { return d_K; }

bool turbo_encoder_impl::set_frame_size(unsigned int frame_bits)
{
    return true;
}

double turbo_encoder_impl::rate() { return static_cast<float>(d_K) / d_N; }

void turbo_encoder_impl::generic_work(const void* inbuffer, void* outbuffer)
{
    const B_8* in = (const B_8*)inbuffer;
    B_8* out = (B_8*)outbuffer;

    d_encoder->encode(in, out);
}
} /* namespace fec_dev */
} /* namespace gr */
