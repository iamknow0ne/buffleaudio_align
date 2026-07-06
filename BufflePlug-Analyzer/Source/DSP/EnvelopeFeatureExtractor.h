#pragma once

#include <vector>

namespace buffle::align
{
struct EnvelopeOptions
{
    int frameSize = 1024;
    int hopSize = 256;
};

class EnvelopeFeatureExtractor
{
public:
    explicit EnvelopeFeatureExtractor (EnvelopeOptions optionsToUse = {});

    std::vector<float> extractRmsEnvelope (const float* samples, int numSamples) const;
    std::vector<float> extractOnsetEnvelope (const std::vector<float>& rmsEnvelope) const;

private:
    EnvelopeOptions options;
};
}
