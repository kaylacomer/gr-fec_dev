/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_TURBO_COMMON_H
#define INCLUDED_FEC_DEV_TURBO_COMMON_H

#include "gnuradio/fec_dev/aff3ct_interleaver.h"

namespace gr {
namespace fec_dev {

namespace Turbo {
typedef enum _enc_standard_t {
    LTE = 0,
    CCSDS,
    CUSTOM
} enc_standard_t;

typedef enum _subenc_implem_t {
    sys = 0,
    generic_sys
} subenc_implem_t;

}

}
}

#endif /* INCLUDED_FEC_DEV_TURBO_COMMON_H */