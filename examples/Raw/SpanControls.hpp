#pragma once

/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <avnd/concepts/parameter.hpp>

#include <numeric>
#include <span>
namespace examples
{
/**
 * Low-pass, but with timestamped controls
 */
struct SpanControls
{
  static consteval auto name() { return "Span example"; }
  static consteval auto c_name() { return "avnd_span"; }
  static consteval auto uuid() { return "e6c34e9e-fc66-44d4-8798-dd3cd67b7fb2"; }

  struct inputs_t
  {
    struct
    {
      static consteval auto name() { return "In"; }

      struct range
      {
        const float min = 0.;
        const float max = 1.;
        const float init = 0.5;
      };

      std::span<float> value{};
    } v;
  } inputs;

  struct
  {
    struct
    {
      static consteval auto name() { return "Out"; }


      float value{};
    } sum;
  } outputs;

  void operator()()
  {
    outputs.sum.value = std::accumulate(begin(inputs.v.value), end(inputs.v.value), 0.f, std::plus<>{});
  }
};
}
