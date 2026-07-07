#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

namespace buffle::align
{

struct RemovedMaterialStats
{
    float amount = 0.0f;
    float deltaRms = 0.0f;
    float peakDelta = 0.0f;
};

RemovedMaterialStats measureRemovedMaterial (const juce::AudioBuffer<float>& original,
                                             const juce::AudioBuffer<float>& processed,
                                             int channelsToMeasure) noexcept;

} // namespace buffle::align
