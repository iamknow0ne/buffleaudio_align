#include "StackSpreadGovernor.h"

namespace buffle::align
{

float getStackSpreadSideGain (float stereoFocus) noexcept
{
    const auto focus = juce::jlimit (0.0f, 1.0f, stereoFocus);
    return juce::jmap (focus, 1.16f, 0.36f);
}

void applyStackSpreadGovernor (juce::AudioBuffer<float>& buffer,
                               int numChannels,
                               float stereoFocus) noexcept
{
    const auto channels = juce::jmin (numChannels, buffer.getNumChannels());
    if (channels < 2 || buffer.getNumSamples() == 0)
        return;

    auto* left = buffer.getWritePointer (0);
    auto* right = buffer.getWritePointer (1);
    const auto sideGain = getStackSpreadSideGain (stereoFocus);

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        const auto mid = (left[sample] + right[sample]) * 0.5f;
        const auto side = (left[sample] - right[sample]) * 0.5f * sideGain;
        left[sample] = mid + side;
        right[sample] = mid - side;
    }
}

} // namespace buffle::align
