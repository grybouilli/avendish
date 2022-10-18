#pragma once

#include <halp/meta.hpp>
#include <halp/custom_widgets.hpp>
#include <halp/layout.hpp>

#include <string>
#include <string_view>
#include <vector>

#include <examples/Advanced/Parametriq/ParametricEqControls.hpp>

/*
* Each tab display has specific controls that depends on the filter response type (Butterworth, CHebyshev I, etc) and the filter type itself (lowpass, highpass, etc). So there should be one tab template per response type.
*/

namespace examples
{
// structures should look like UIBand from ParametricEQ but should be adapted to the filter type. Needed controls are specified in comments.
struct ButterworthTypeITab
{
    // filter order control, cutoffFreq knob
};

struct ButterworthTypeIITab
{
    // filter order control, center frequency knob, frequency band knob
};

struct ButterworthTypeIIITab
{
    // filter order control, center frequency knob, shelf gain knob
};

struct ButterworthTypeIVTab
{
    // filter order control, center frequency knob, frequency band know, shelf gain knob
};

/*
    Each band can have one instance of each control type (cutoffFreq knob, gain knob, etc) and each tab should refer to the needed ones. This way, it avoids having too many unused controls in memory. For example, ButterworthTypeITab from band n°1 should use the filter control and cutoffFreq know n°1. Those will also be used by ChebyshevITypeITab for example. 
*/
}