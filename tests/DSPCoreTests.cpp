#include "DSP/EnvelopeFeatureExtractor.h"
#include "DSP/ManualNudgeDelay.h"
#include "DSP/TimingOffsetEstimator.h"

#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

namespace
{
void testSilenceEnvelopeIsZero()
{
    std::vector<float> samples (4096, 0.0f);
    buffle::align::EnvelopeFeatureExtractor extractor ({ 512, 128 });
    const auto envelope = extractor.extractRmsEnvelope (samples.data(), static_cast<int> (samples.size()));

    assert (! envelope.empty());
    for (const auto value : envelope)
        assert (value == 0.0f);
}

void testImpulseProducesOnset()
{
    std::vector<float> samples (4096, 0.0f);
    samples[1024] = 1.0f;

    buffle::align::EnvelopeFeatureExtractor extractor ({ 256, 128 });
    const auto rms = extractor.extractRmsEnvelope (samples.data(), static_cast<int> (samples.size()));
    const auto onset = extractor.extractOnsetEnvelope (rms);

    auto hasOnset = false;
    for (const auto value : onset)
        hasOnset = hasOnset || value > 0.001f;

    assert (hasOnset);
}

void testShiftedClickOffset()
{
    std::vector<float> guide (32, 0.0f);
    std::vector<float> dub (32, 0.0f);
    guide[10] = 1.0f;
    dub[13] = 1.0f;

    buffle::align::TimingOffsetEstimator estimator;
    const auto result = estimator.estimate (guide, dub, 8, 5.0f);

    assert (result.frameOffset == 3);
    assert (std::abs (result.milliseconds - 15.0f) < 0.001f);
    assert (result.confidence > 0.99f);
}

void testEqualClickOffsetIsZero()
{
    std::vector<float> guide (32, 0.0f);
    std::vector<float> dub (32, 0.0f);
    guide[10] = 1.0f;
    dub[10] = 1.0f;

    buffle::align::TimingOffsetEstimator estimator;
    const auto result = estimator.estimate (guide, dub, 8, 5.0f);

    assert (result.frameOffset == 0);
    assert (std::abs (result.milliseconds) < 0.001f);
    assert (result.confidence > 0.99f);
}

void testSilenceOffsetHasLowConfidence()
{
    std::vector<float> guide (32, 0.0f);
    std::vector<float> dub (32, 0.0f);

    buffle::align::TimingOffsetEstimator estimator;
    const auto result = estimator.estimate (guide, dub, 8, 5.0f);

    assert (result.confidence == 0.0f);
}

void testManualNudgeZeroDelayIsIdentity()
{
    juce::AudioBuffer<float> buffer (1, 8);
    for (int i = 0; i < buffer.getNumSamples(); ++i)
        buffer.setSample (0, i, static_cast<float> (i + 1));

    buffle::align::ManualNudgeDelay delay;
    delay.prepare (1, 16);
    delay.process (buffer, 1, 0);

    for (int i = 0; i < buffer.getNumSamples(); ++i)
        assert (buffer.getSample (0, i) == static_cast<float> (i + 1));
}

void testManualNudgeDelaysImpulse()
{
    juce::AudioBuffer<float> buffer (1, 8);
    buffer.clear();
    buffer.setSample (0, 0, 1.0f);

    buffle::align::ManualNudgeDelay delay;
    delay.prepare (1, 16);
    delay.process (buffer, 1, 3);

    assert (buffer.getSample (0, 0) == 0.0f);
    assert (buffer.getSample (0, 1) == 0.0f);
    assert (buffer.getSample (0, 2) == 0.0f);
    assert (buffer.getSample (0, 3) == 1.0f);
}
}

int main()
{
    testSilenceEnvelopeIsZero();
    testImpulseProducesOnset();
    testShiftedClickOffset();
    testEqualClickOffsetIsZero();
    testSilenceOffsetHasLowConfidence();
    testManualNudgeZeroDelayIsIdentity();
    testManualNudgeDelaysImpulse();

    std::cout << "Buffle Align DSP tests passed\n";
    return 0;
}
