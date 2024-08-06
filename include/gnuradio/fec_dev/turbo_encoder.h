/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_TURBO_ENCODER_H
#define INCLUDED_FEC_DEV_TURBO_ENCODER_H

#include <gnuradio/fec/api.h>
#include <gnuradio/fec/generic_encoder.h>
#include <gnuradio/fec_dev/turbo_common.h>
#include <map>
#include <string>

namespace gr {
namespace fec_dev {

/*!
* \brief Turbo Encoding class.
* \ingroup error_coding_blk
*
* \details
* A turbo encoder class that simply passes the input to the
* output. It is meant to allow us to easily use the FEC API
* encoder and decoder blocks in an application with no coding.
*/
class FEC_API turbo_encoder : virtual public fec::generic_encoder
{
public:
    /*!
    * Build a turbo encoding FEC API object.
    *
    * \param frame_size Number of bits per frame. If using in the
    *        tagged stream style, this is the maximum allowable
    *        number of bits per frame.
    * \param standard Select a standard (LTE or CCSDS) to
    *        automatically set some parameters. kmc
    * \param subencoder 
    */
    static generic_encoder::sptr make(int frame_size,
                                      interleaver_t standard=LTE,
                                      enc_sub_type_t subencoder=RSC,
                                      bool buffered=true,
                                      std::vector<int> polys={013,015},
                                      int trellis_size = 8);

    /*!
    * Sets the uncoded frame size to \p frame_size. If \p
    * frame_size is greater than the value given to the
    * constructor, the frame size will be capped by that initial
    * value and this function will return false. Otherwise, it
    * returns true.
    */
    bool set_frame_size(unsigned int frame_size) override = 0;

    /*!
    * Returns the coding rate of this encoder (it will always be 1).
    */
    double rate() override = 0;
};
    
} // namespace fec_dev
} // namespace gr

#endif /* INCLUDED_FEC_DEV_TURBO_ENCODER_H */
