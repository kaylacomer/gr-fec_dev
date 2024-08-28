/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_AFF3CT_DECODER_H
#define INCLUDED_FEC_DEV_AFF3CT_DECODER_H

namespace gr {
namespace fec_dev {

namespace Quantizer {
typedef enum _quantizer_impl_t {
    STD = 0,
    FAST,
    NO
} quantizer_impl_t;
}

namespace Decoder {
typedef enum _decoder_impl_t {
    STD = 0,
    FAST,
    GENIUS,
    VERY_FAST,
    NAIVE
} decoder_impl_t;
}

namespace SIMD {
typedef enum _simd_strat_t {
    SEQ = 0,
    INTER,
    INTRA,
    INTER_INTRA
} simd_strat_t;
}

}
}

#endif /* INCLUDED_FEC_DEV_AFF3CT_DECODER_H */