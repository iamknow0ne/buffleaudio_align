#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

namespace buffle::align
{

float getStackSpreadSideGain (float stereoFocus) noexcept;

void applyStackSpreadGovernor (juce::AudioBuffer<float>& buffer,
                               int numChannels,
                               float stereoFocus) noexcept;

} // namespace buffle::align
