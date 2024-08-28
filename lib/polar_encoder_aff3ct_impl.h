/* -*- c++ -*- */
/*
 * Copyright 2024 gr-fec_dev author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FEC_DEV_POLAR_ENCODER_AFF3CT_IMPL_H
#define INCLUDED_FEC_DEV_POLAR_ENCODER_AFF3CT_IMPL_H

#include <gnuradio/fec_dev/polar_encoder_aff3ct.h>
#include <map>
#include <string>

#include "Tools/types.h"
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

namespace gr {
namespace fec_dev {
    
class FEC_API polar_encoder_aff3ct_impl : public polar_encoder_aff3ct
{
private:
  unsigned int d_K;
  int d_N;
  std::unique_ptr<aff3ct::module::Encoder_polar<B_8>> d_encoder;
  std::unique_ptr<aff3ct::tools::Frozenbits_generator> d_frozen_bitgen;

public:
  polar_encoder_aff3ct_impl(int K,
                            int N,
                            Polar::frozen_bit_gen_t frozen_bit_gen = Polar::GA_ARIKAN,
                            Polar::noise_t noise_type = Polar::Sigma);
  ~polar_encoder_aff3ct_impl() override;

  bool set_frame_size(unsigned int frame_size) override;
  double rate() override;
  int get_output_size() override;
  int get_input_size() override;
  void generic_work(const void* inbuffer, void* outbuffer) override;
};

} // namespace fec_dev
} // namespace gr

#endif /* INCLUDED_FEC_DEV_POLAR_ENCODER_AFF3CT_IMPL_H */