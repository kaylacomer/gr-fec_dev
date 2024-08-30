/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_POLAR_DECODER_AFF3CT_IMPL_H
#define INCLUDED_FEC_DEV_POLAR_DECODER_AFF3CT_IMPL_H

#include <gnuradio/fec_dev/polar_decoder_aff3ct.h>
#include <map>
#include <string>

#include "Tools/types.h"
#include "Module/Decoder/Polar/SC/Decoder_polar_SC_fast_sys.hpp"
#include "Module/Decoder/Polar/SC/Decoder_polar_SC_naive_sys.hpp"
#include "Tools/Code/Polar/API/API_polar_static_intra_8bit.hpp"
#include <Module/Quantizer/Pow2/Quantizer_pow2.hpp>
#include <Module/Quantizer/Pow2/Quantizer_pow2_fast.hpp>
#include "Module/Encoder/Polar/Encoder_polar_sys.hpp"
#include "Tools/Code/Polar/Frozenbits_generator/Frozenbits_generator.hpp"
#include "Tools/Code/Polar/Frozenbits_generator/Frozenbits_generator_5G.hpp"
#include "Tools/Code/Polar/Frozenbits_generator/Frozenbits_generator_BEC.hpp"
#include "Tools/Code/Polar/Frozenbits_generator/Frozenbits_generator_GA_Arikan.hpp"
#include "Tools/Code/Polar/Frozenbits_generator/Frozenbits_generator_GA.hpp"
#include "Tools/Code/Polar/Frozenbits_generator/Frozenbits_generator_TV.hpp"
#include "Tools/Interface/Interface_get_set_noise.hpp"
#include "Tools/Noise/Noise.hpp"
#include "Tools/Noise/Sigma.hpp"

#include "aff3ct_quantizer_headers.h"

namespace gr {
namespace fec_dev {
    
class FEC_API polar_decoder_aff3ct_impl : public polar_decoder_aff3ct
{
private:
  unsigned int d_K;
  int d_N;
  std::unique_ptr<aff3ct::tools::Frozenbits_generator> d_frozen_bitgen;
  std::unique_ptr<aff3ct::module::Decoder_SIHO<B_8, Q_8>> d_decoder;
  std::vector<float> d_tmp_input;
  std::vector<Q_8> d_quant_input;
  std::unique_ptr<aff3ct::module::Quantizer<float, Q_8>> d_quant;

public:
  polar_decoder_aff3ct_impl(int K,
                            int N,
                            float sigma,
                            Polar::frozen_bit_gen_t frozen_bit_gen = Polar::GA_ARIKAN,
                            Polar::noise_t noise_type = Polar::Sigma,
                            Polar::decoder_t decoder_type = Polar::SC,
                            Decoder::decoder_impl_t dec_impl = Decoder::NAIVE,
                            uint8_t quant_fixed_point_pos = 2,
                            uint8_t quant_saturation_pos = 6,
                            Quantizer::quantizer_impl_t quant_impl=Quantizer::STD
                            );
  ~polar_decoder_aff3ct_impl() override;

  bool set_frame_size(unsigned int frame_size) override;
  double rate() override;
  int get_output_size() override;
  int get_input_size() override;
  void generic_work(const void* inbuffer, void* outbuffer) override;
};

} // namespace fec_dev
} // namespace gr

#endif /* INCLUDED_FEC_DEV_POLAR_DECODER_AFF3CT_IMPL_H */