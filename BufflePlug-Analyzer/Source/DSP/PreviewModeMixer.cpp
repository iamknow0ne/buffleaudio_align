#include "PreviewModeMixer.h"

namespace buffle::align
{
void renderPreviewMode (juce::AudioBuffer<float>& processed,
                        const juce::AudioBuffer<float>& original,
                        PreviewMode mode,
                        int channelsToRender)
{
    const auto channels = juce::jmin (processed.getNumChannels(),
                                      original.getNumChannels(),
                                      juce::jmax (0, channelsToRender));
    const auto samples = juce::jmin (processed.getNumSamples(), original.getNumSamples());

    if (channels == 0 || samples == 0 || mode == PreviewMode::aligned)
        return;

    for (int channel = 0; channel < channels; ++channel)
    {
        auto* processedData = processed.getWritePointer (channel);
        const auto* originalData = original.getReadPointer (channel);

        if (mode == PreviewMode::original)
        {
            juce::FloatVectorOperations::copy (processedData, originalData, samples);
            continue;
        }

        if (mode == PreviewMode::difference)
            juce::FloatVectorOperations::subtract (processedData, originalData, samples);
    }
}
}
