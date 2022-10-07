#pragma once
#include <cmath>
#include <halp/audio.hpp>
#include <halp/controls.hpp>
#include <halp/meta.hpp>

namespace examples::helpers
{
/**
 * A simple example of audio analysis.
 * Takes an input channel, writes the peak absolute value.
 */
struct Peak
{
  halp_meta(name, "Peak value")
  halp_meta(c_name, "avnd_peak")
  halp_meta(uuid, "57d3476d-9dbb-45ac-b76e-a2a51b48b8af")

  struct
  {
    halp::audio_channel<"In", double> audio;
  } inputs;

  struct
  {
    halp::val_port<"Peak", double> peak;
  } outputs;

  void operator()(int frames)
  {
    outputs.peak = 0.;
    for(int k = 0; k < frames; k++)
    {
      double sample = std::abs(inputs.audio[k]);
      if(sample > outputs.peak)
        outputs.peak = sample;
    }
  }
};

}
