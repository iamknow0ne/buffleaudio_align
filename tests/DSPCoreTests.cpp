#include "DSP/ConsonantTamer.h"
#include "DSP/EnvelopeFeatureExtractor.h"
#include "DSP/ManualNudgeDelay.h"
#include "DSP/PreviewModeMixer.h"
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

void testConsonantTamerAmountZeroIsIdentity()
{
    juce::AudioBuffer<float> buffer (1, 128);
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        buffer.setSample (0, sample, sample == 24 ? 1.0f : 0.08f);

    auto original = buffer;

    buffle::align::ConsonantTamer tamer;
    tamer.prepare (44100.0, 1);
    tamer.process (buffer, nullptr, 0.0f, 0.5f);

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        assert (buffer.getSample (0, sample) == original.getSample (0, sample));
}

void testConsonantTamerSilenceStaysSilent()
{
    juce::AudioBuffer<float> buffer (2, 128);
    buffer.clear();

    buffle::align::ConsonantTamer tamer;
    tamer.prepare (44100.0, 2);
    tamer.process (buffer, nullptr, 1.0f, 0.35f);

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            assert (buffer.getSample (channel, sample) == 0.0f);
}

void testConsonantTamerReducesDubOnlyBurst()
{
    juce::AudioBuffer<float> buffer (1, 4096);
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        buffer.setSample (0, sample, 0.06f);

    buffer.setSample (0, 48, 1.0f);
    buffer.setSample (0, 49, -0.8f);

    buffle::align::ConsonantTamer tamer;
    tamer.prepare (44100.0, 1);
    tamer.process (buffer, nullptr, 1.0f, 0.2f);

    assert (std::abs (buffer.getSample (0, 48)) < 0.92f);
    assert (std::abs (buffer.getSample (0, 3000) - 0.06f) < 0.006f);
}

void testConsonantTamerPreservesSustainedVowel()
{
    juce::AudioBuffer<float> buffer (1, 4096);
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        buffer.setSample (0, sample, 0.12f * std::sin (static_cast<float> (sample) * 0.03f));

    auto before = buffer.getRMSLevel (0, 2048, 1024);

    buffle::align::ConsonantTamer tamer;
    tamer.prepare (44100.0, 1);
    tamer.process (buffer, nullptr, 1.0f, 0.85f);

    auto after = buffer.getRMSLevel (0, 2048, 1024);
    assert (std::abs (juce::Decibels::gainToDecibels (after / before)) < 0.5f);
}

void testConsonantTamerPreservesGuideMatchedAttack()
{
    juce::AudioBuffer<float> dubOnly (1, 256);
    juce::AudioBuffer<float> matchedDub (1, 256);
    juce::AudioBuffer<float> guide (1, 256);
    dubOnly.clear();
    matchedDub.clear();
    guide.clear();

    dubOnly.setSample (0, 48, 1.0f);
    matchedDub.setSample (0, 48, 1.0f);
    guide.setSample (0, 48, 1.0f);

    buffle::align::ConsonantTamer dubOnlyTamer;
    dubOnlyTamer.prepare (44100.0, 1);
    dubOnlyTamer.process (dubOnly, nullptr, 1.0f, 0.2f);

    buffle::align::ConsonantTamer matchedTamer;
    matchedTamer.prepare (44100.0, 1);
    matchedTamer.process (matchedDub, &guide, 1.0f, 0.2f);

    assert (matchedDub.getSample (0, 48) > dubOnly.getSample (0, 48));
}

void testPreviewModeOriginalRestoresInput()
{
    juce::AudioBuffer<float> processed (1, 4);
    juce::AudioBuffer<float> original (1, 4);

    for (int sample = 0; sample < 4; ++sample)
    {
        original.setSample (0, sample, static_cast<float> (sample + 1));
        processed.setSample (0, sample, static_cast<float> ((sample + 1) * 10));
    }

    buffle::align::renderPreviewMode (processed, original, buffle::align::PreviewMode::original, 1);

    for (int sample = 0; sample < 4; ++sample)
        assert (processed.getSample (0, sample) == original.getSample (0, sample));
}

void testPreviewModeAlignedKeepsProcessed()
{
    juce::AudioBuffer<float> processed (1, 4);
    juce::AudioBuffer<float> original (1, 4);

    for (int sample = 0; sample < 4; ++sample)
    {
        original.setSample (0, sample, static_cast<float> (sample + 1));
        processed.setSample (0, sample, static_cast<float> ((sample + 1) * 10));
    }

    buffle::align::renderPreviewMode (processed, original, buffle::align::PreviewMode::aligned, 1);

    for (int sample = 0; sample < 4; ++sample)
        assert (processed.getSample (0, sample) == static_cast<float> ((sample + 1) * 10));
}

void testPreviewModeDifferenceShowsChange()
{
    juce::AudioBuffer<float> processed (1, 4);
    juce::AudioBuffer<float> original (1, 4);

    for (int sample = 0; sample < 4; ++sample)
    {
        original.setSample (0, sample, static_cast<float> (sample + 1));
        processed.setSample (0, sample, static_cast<float> ((sample + 1) * 3));
    }

    buffle::align::renderPreviewMode (processed, original, buffle::align::PreviewMode::difference, 1);

    for (int sample = 0; sample < 4; ++sample)
        assert (processed.getSample (0, sample) == static_cast<float> ((sample + 1) * 2));
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
    testConsonantTamerAmountZeroIsIdentity();
    testConsonantTamerSilenceStaysSilent();
    testConsonantTamerReducesDubOnlyBurst();
    testConsonantTamerPreservesSustainedVowel();
    testConsonantTamerPreservesGuideMatchedAttack();
    testPreviewModeOriginalRestoresInput();
    testPreviewModeAlignedKeepsProcessed();
    testPreviewModeDifferenceShowsChange();

    std::cout << "Buffle Align DSP tests passed\n";
    return 0;
}
