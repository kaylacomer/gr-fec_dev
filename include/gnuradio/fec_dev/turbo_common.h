/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_TURBO_COMMON_H
#define INCLUDED_FEC_DEV_TURBO_COMMON_H

#include "gnuradio/fec_dev/aff3ct_common.h" //interleaver types

namespace gr {
namespace fec_dev {

typedef enum _enc_sub_type_t {
    RSC = 0
} enc_sub_type_t;

}
}

#endif /* INCLUDED_FEC_DEV_TURBO_COMMON_H */