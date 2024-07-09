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
#include <volk/volk.h>
#include <sstream>
#include <vector>

#include "Tools/Interleaver/LTE/Interleaver_core_LTE.hpp"
#include "Tools/Interleaver/CCSDS/Interleaver_core_CCSDS.hpp"
#include "Tools/Interleaver/Column_row/Interleaver_core_column_row.hpp"
#include "Tools/Interleaver/Golden/Interleaver_core_golden.hpp"
#include "Tools/Interleaver/NO/Interleaver_core_NO.hpp"
#include "Tools/Interleaver/Random/Interleaver_core_random.hpp"
#include "Tools/Interleaver/Random_column/Interleaver_core_random_column.hpp"
#include "Tools/Interleaver/Row_column/Interleaver_core_row_column.hpp"

namespace gr {
namespace fec_dev {

fec::generic_encoder::sptr turbo_encoder::make(int frame_size,
                                               enc_standard_t standard,
                                               enc_sub_type_t subencoder,
                                               bool buffered,
                                               std::vector<int> polys,
                                               int trellis_size)
{
    return fec::generic_encoder::sptr(new turbo_encoder_impl(
        frame_size, standard, subencoder, buffered, polys, trellis_size));
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
    : generic_encoder("turbo_encoder"),
      d_frame_size(frame_size),
      d_trellis_size(trellis_size)
{
    if (standard == LTE) {
        d_logger->info("Encoding to LTE standard (8-stage trellis, [013, 015] polynomials, LTE interleaver)");

        d_trellis_size = 8;
        polys = {013,015};

        d_K = d_frame_size;

        d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_LTE<>>(d_K);
    }
    else if (standard == CCSDS) {
        d_logger->info("Encoding to CCSDS standard (16-stage trellis, [023, 033] polynomials, CCSDS interleaver)");

        d_trellis_size = 16;
        polys = {023,033};

        if (d_frame_size > 8920) {
            throw std::runtime_error("maximum frame size for CCSDS is 8920");
        }

        std::vector<int> CCSDS_sizes = {1784, 3568, 7136, 8920};
        for (const int& size : CCSDS_sizes) {
            if (d_frame_size <= size) {
                d_K = size;
                break;
            }
        }
        
        if (d_K - d_frame_size > 0) {
            d_logger->info("CCSDS supported frame sizes are 1784, 3568, 7136, 8920. Padding {:d}-bit input with {:d} zeros, which is inefficient", d_frame_size, d_K - d_frame_size);
        }

        d_interleaver_core = std::make_unique<aff3ct::tools::Interleaver_core_CCSDS<>>(d_K);
    }
    else {
        d_logger->info("Custom encoding: {:d}-stage trellis, __ polynomials, random interleaver", d_trellis_size);
        d_K = d_frame_size;
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
int turbo_encoder_impl::get_input_size() { return d_frame_size; }

bool turbo_encoder_impl::set_frame_size(unsigned int frame_size)
{
    return true;
}

double turbo_encoder_impl::rate() { return d_frame_size / d_N; }

void turbo_encoder_impl::generic_work(const void* inbuffer, void* outbuffer)
{
    const B_8* in = (const B_8*)inbuffer;
    B_8* out = (B_8*)outbuffer;

    std::vector<B_8> input_vector(in, in + d_frame_size);
    input_vector.resize(d_K, 0);

    d_encoder->encode(input_vector.data(), out);
}
} /* namespace fec_dev */
} /* namespace gr */
