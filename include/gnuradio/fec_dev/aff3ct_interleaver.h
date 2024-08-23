/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_AFF3CT_INTERLEAVER_H
#define INCLUDED_FEC_DEV_AFF3CT_INTERLEAVER_H

namespace gr {
namespace fec_dev {

namespace Interleaver {
typedef enum _interleaver_t {
    LTE = 0,
    CCSDS,
    NO,
    COL_ROW,
    ROW_COL,
    RANDOM,
    RAND_COL,
    GOLDEN,
    DVB_RCS1,
    DVB_RCS2
} interleaver_t;


typedef enum _itl_read_order_t {
    TOP_LEFT = 0, // read from the top row by row from left to right
    TOP_RIGHT,    // read from the top row by row from right to left
    BOTTOM_LEFT,  // read from the bottom row by row from left to right
    BOTTOM_RIGHT,  // read from the bottom row by row from right to left
    NA = -1 // not applicable -- parameter included but not relevant
} itl_read_order_t;
}

}
}

#endif /* INCLUDED_FEC_DEV_AFF3CT_INTERLEAVER_H */