/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_BCJR_H
#define INCLUDED_FEC_DEV_BCJR_H

#include "gnuradio/fec_dev/aff3ct_interleaver.h"

namespace gr {
namespace fec_dev {

namespace BCJR {
typedef enum _bcjr_impl_t {
    STD = 0,
    FAST,
    VERY_FAST,
    SCAN,
    GENERIC
} bcjr_impl_t;
}

namespace SIMD {
typedef enum _simd_interintra_impl_t {
    X2_AVX = 0,
    X2_SSE,
    X4_AVX,
    NA = -1
} simd_interintra_impl_t;
}

}
}

#endif /* INCLUDED_FEC_DEV_BCJR_H */