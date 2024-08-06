/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_AFF3CT_COMMON_H
#define INCLUDED_FEC_DEV_AFF3CT_COMMON_H

#include "Tools/Interleaver/NO/Interleaver_core_NO.hpp"
#include "Tools/Interleaver/Column_row/Interleaver_core_column_row.hpp"
#include "Tools/Interleaver/Row_column/Interleaver_core_row_column.hpp"
#include "Tools/Interleaver/Random/Interleaver_core_random.hpp"
#include "Tools/Interleaver/Random_column/Interleaver_core_random_column.hpp"
#include "Tools/Interleaver/Golden/Interleaver_core_golden.hpp"
#include "Tools/Interleaver/CCSDS/Interleaver_core_CCSDS.hpp"
#include "Tools/Interleaver/LTE/Interleaver_core_LTE.hpp"
#include "Tools/Interleaver/ARP/Interleaver_core_ARP_DVB_RCS1.hpp"
#include "Tools/Interleaver/ARP/Interleaver_core_ARP_DVB_RCS2.hpp"


namespace gr {
namespace fec_dev {

typedef enum _interleaver_t {
    NO = 0,
    COL_ROW,
    ROW_COL,
    RANDOM,
    RAND_COL,
    GOLDEN,
    CCSDS,
    LTE,
    DVB_RCS1,
    DVB_RCS2
} interleaver_t;

typedef enum _itl_read_order_t {
    TOP_LEFT,    // read from the top row by row from left to right
    TOP_RIGHT,   // read from the top row by row from right to left
    BOTTOM_LEFT, // read from the bottom row by row from left to right
    BOTTOM_RIGHT // read from the bottom row by row from right to left
} itl_read_order_t;

}
}

#endif /* INCLUDED_FEC_DEV_AFF3CT_COMMON_H */