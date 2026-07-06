#include "EnvelopeFeatureExtractor.h"

#include <algorithm>
#include <cmath>

namespace buffle::align
{
EnvelopeFeatureExtractor::EnvelopeFeatureExtractor (EnvelopeOptions optionsToUse)
    : options (optionsToUse)
{
    options.frameSize = std::max (1, options.frameSize);
    options.hopSize = std::max (1, options.hopSize);
}

std::vector<float> EnvelopeFeatureExtractor::extractRmsEnvelope (const float* samples, int numSamples) const
{
    if (samples == nullptr || numSamples <= 0)
        return {};

    std::vector<float> envelope;
    envelope.reserve (static_cast<size_t> (numSamples / options.hopSize + 1));

    for (int start = 0; start < numSamples; start += options.hopSize)
    {
        const auto end = std::min (numSamples, start + options.frameSize);
        double sumSquares = 0.0;

        for (int i = start; i < end; ++i)
            sumSquares += static_cast<double> (samples[i]) * static_cast<double> (samples[i]);

        const auto frameLength = std::max (1, end - start);
        envelope.push_back (static_cast<float> (std::sqrt (sumSquares / static_cast<double> (frameLength))));
    }

    return envelope;
}

std::vector<float> EnvelopeFeatureExtractor::extractOnsetEnvelope (const std::vector<float>& rmsEnvelope) const
{
    if (rmsEnvelope.empty())
        return {};

    std::vector<float> onset;
    onset.reserve (rmsEnvelope.size());
    onset.push_back (0.0f);

    for (size_t i = 1; i < rmsEnvelope.size(); ++i)
        onset.push_back (std::max (0.0f, rmsEnvelope[i] - rmsEnvelope[i - 1]));

    return onset;
}
}
