#include "RemovedMaterialMeter.h"

#include <algorithm>
#include <cmath>

namespace buffle::align
{

RemovedMaterialStats measureRemovedMaterial (const juce::AudioBuffer<float>& original,
                                             const juce::AudioBuffer<float>& processed,
                                             int channelsToMeasure) noexcept
{
    const auto channels = juce::jmin (original.getNumChannels(),
                                      processed.getNumChannels(),
                                      juce::jmax (0, channelsToMeasure));
    const auto samples = juce::jmin (original.getNumSamples(), processed.getNumSamples());

    if (channels == 0 || samples == 0)
        return {};

    double deltaSquares = 0.0;
    double originalSquares = 0.0;
    auto peakDelta = 0.0f;
    auto sampleCount = 0;

    for (int channel = 0; channel < channels; ++channel)
    {
        const auto* originalData = original.getReadPointer (channel);
        const auto* processedData = processed.getReadPointer (channel);

        for (int sample = 0; sample < samples; ++sample)
        {
            const auto originalSample = originalData[sample];
            const auto delta = processedData[sample] - originalSample;
            deltaSquares += static_cast<double> (delta) * static_cast<double> (delta);
            originalSquares += static_cast<double> (originalSample) * static_cast<double> (originalSample);
            peakDelta = juce::jmax (peakDelta, std::abs (delta));
            ++sampleCount;
        }
    }

    if (sampleCount == 0)
        return {};

    const auto deltaRms = static_cast<float> (std::sqrt (deltaSquares / static_cast<double> (sampleCount)));
    const auto originalRms = static_cast<float> (std::sqrt (originalSquares / static_cast<double> (sampleCount)));
    const auto amount = originalRms > 1.0e-5f ? deltaRms / originalRms : 0.0f;

    return { juce::jlimit (0.0f, 1.0f, amount),
             juce::jlimit (0.0f, 1.0f, deltaRms),
             juce::jlimit (0.0f, 1.0f, peakDelta) };
}

} // namespace buffle::align
