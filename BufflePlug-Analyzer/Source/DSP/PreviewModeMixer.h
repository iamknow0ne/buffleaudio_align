#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

namespace buffle::align
{
enum class PreviewMode
{
    original = 0,
    aligned = 1,
    difference = 2,
    consonantRemoved = 3
};

void renderPreviewMode (juce::AudioBuffer<float>& processed,
                        const juce::AudioBuffer<float>& original,
                        PreviewMode mode,
                        int channelsToRender);
}
