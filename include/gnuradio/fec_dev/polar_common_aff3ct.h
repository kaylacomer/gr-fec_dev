/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_POLAR_COMMON_AFF3CT_H
#define INCLUDED_FEC_DEV_POLAR_COMMON_AFF3CT_H

namespace gr {
namespace fec_dev {

namespace Polar {
typedef enum _frozen_bit_gen_t {
    GA_ARIKAN = 0,
    FIVE_G,
    BEC,
    GA,
    TV
} frozen_bit_gen_t;

typedef enum _decoder_t {
    SC = 0,
    ASCL,
    SCAN,
    SCF,
    SCL
} decoder_t;

typedef enum _noise_t {
    Sigma = 0,
    Received_optical_power,
    Event_probability
} noise_t;

}

}
}

#endif /* INCLUDED_FEC_DEV_POLAR_COMMON_AFF3CT_H */