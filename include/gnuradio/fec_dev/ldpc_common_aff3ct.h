/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_LDPC_COMMON_AFF3CT_H
#define INCLUDED_FEC_DEV_LDPC_COMMON_AFF3CT_H

namespace gr {
namespace fec_dev {

namespace LDPC {
typedef enum _encoder_t {
    LDPC = 0,
    LDPC_H,
    LDPC_DVBS2,
    LDPC_IRA,
    LDPC_QC
} encoder_t;

typedef enum _enc_gen_matrix_method_t {
    IDENTITY = 0,
    LU_DEC
} enc_gen_matrix_method_t;

typedef enum _decoder_t {
    BIT_FLIPPING = 0,
    BP_PEELING,
    BP_FLOODING,
    BP_HORIZ_LAYER,
    BP_VERT_LAYER
} decoder_t;

typedef enum _dec_impl_t {
    STD = 0,
    GALA,
    GALB,
    GALE,
    PPBF,
    WBF,
    MWBF,
    SPA,
    LSPA,
    AMS,
    MS,
    NMS,
    OMS
} dec_impl_t;

typedef enum _dec_H_reorder_t {
    ASC = 0,
    DSC,
    NONE
} dec_H_reorder_t;

typedef enum _dec_min_AMS_t {
    MINS = 0,
    MINL,
    MIN
} dec_min_AMS_t;

}

}
}

#endif /* INCLUDED_FEC_DEV_LDPC_COMMON_AFF3CT_H */