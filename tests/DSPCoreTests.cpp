#include "DSP/AlignmentReport.h"
#include "DSP/BidirectionalNudge.h"
#include "DSP/ConsonantTamer.h"
#include "DSP/EnvelopeFeatureExtractor.h"
#include "DSP/ManualNudgeDelay.h"
#include "DSP/NaturalnessGuardrail.h"
#include "DSP/PreviewModeMixer.h"
#include "DSP/RemovedMaterialMeter.h"
#include "DSP/StackRolePreset.h"
#include "DSP/TimingOffsetEstimator.h"
#include "DSP/TrustDiagnostics.h"

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

void testReverseShiftedClickOffset()
{
    std::vector<float> guide (32, 0.0f);
    std::vector<float> dub (32, 0.0f);
    guide[13] = 1.0f;
    dub[10] = 1.0f;

    buffle::align::TimingOffsetEstimator estimator;
    const auto result = estimator.estimate (guide, dub, 8, 5.0f);

    assert (result.frameOffset == -3);
    assert (std::abs (result.milliseconds + 15.0f) < 0.001f);
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

void testBidirectionalNudgePlanUsesLatencyCentre()
{
    const auto plan = buffle::align::calculateBidirectionalNudgePlan (0.0f, 1000.0, 120.0f, 241);
    assert (plan.hostLatencySamples == 120);
    assert (plan.delaySamples == 120);
}

void testBidirectionalNudgePlanAllowsNegativeAndPositiveMoves()
{
    const auto early = buffle::align::calculateBidirectionalNudgePlan (-20.0f, 1000.0, 120.0f, 241);
    const auto late = buffle::align::calculateBidirectionalNudgePlan (20.0f, 1000.0, 120.0f, 241);

    assert (early.hostLatencySamples == 120);
    assert (early.delaySamples == 100);
    assert (late.delaySamples == 140);
}

void testBidirectionalNudgePlanClampsAtLatencyEdges()
{
    const auto advanceLimit = buffle::align::calculateBidirectionalNudgePlan (-250.0f, 1000.0, 120.0f, 241);
    const auto delayLimit = buffle::align::calculateBidirectionalNudgePlan (250.0f, 1000.0, 120.0f, 241);

    assert (advanceLimit.hostLatencySamples == 120);
    assert (advanceLimit.delaySamples == 0);
    assert (delayLimit.hostLatencySamples == 120);
    assert (delayLimit.delaySamples == 240);
}

void testBidirectionalSuggestionInvertsOffset()
{
    assert (buffle::align::suggestBidirectionalNudgeMs (35.0f, 120.0f) == -35.0f);
    assert (buffle::align::suggestBidirectionalNudgeMs (-22.5f, 120.0f) == 22.5f);
    assert (buffle::align::suggestBidirectionalNudgeMs (240.0f, 120.0f) == -120.0f);
}

float consonantFixtureSample (int sample)
{
    const auto body = 0.08f * std::sin (static_cast<float> (sample) * 0.037f);
    const auto burstA = sample == 48 ? 1.0f : sample == 49 ? -0.72f : 0.0f;
    const auto burstB = sample == 300 ? 0.84f : sample == 301 ? -0.62f : 0.0f;
    return body + burstA + burstB;
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

void testConsonantTamerMatchesSingleBlockAndChunkedProcessing()
{
    juce::AudioBuffer<float> full (1, 512);
    juce::AudioBuffer<float> firstHalf (1, 256);
    juce::AudioBuffer<float> secondHalf (1, 256);

    for (int sample = 0; sample < full.getNumSamples(); ++sample)
    {
        const auto value = consonantFixtureSample (sample);
        full.setSample (0, sample, value);

        if (sample < 256)
            firstHalf.setSample (0, sample, value);
        else
            secondHalf.setSample (0, sample - 256, value);
    }

    buffle::align::ConsonantTamer fullTamer;
    fullTamer.prepare (44100.0, 1);
    fullTamer.process (full, nullptr, 0.82f, 0.38f);

    buffle::align::ConsonantTamer chunkedTamer;
    chunkedTamer.prepare (44100.0, 1);
    chunkedTamer.process (firstHalf, nullptr, 0.82f, 0.38f);
    chunkedTamer.process (secondHalf, nullptr, 0.82f, 0.38f);

    for (int sample = 0; sample < full.getNumSamples(); ++sample)
    {
        const auto chunked = sample < 256 ? firstHalf.getSample (0, sample)
                                          : secondHalf.getSample (0, sample - 256);
        assert (std::abs (full.getSample (0, sample) - chunked) < 0.0001f);
    }
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

void testPreviewModeConsonantRemovedShowsScopedChange()
{
    juce::AudioBuffer<float> processed (1, 4);
    juce::AudioBuffer<float> preTamer (1, 4);

    for (int sample = 0; sample < 4; ++sample)
    {
        preTamer.setSample (0, sample, static_cast<float> (sample + 1));
        processed.setSample (0, sample, static_cast<float> ((sample + 1) * 0.75f));
    }

    buffle::align::renderPreviewMode (processed, preTamer, buffle::align::PreviewMode::consonantRemoved, 1);

    for (int sample = 0; sample < 4; ++sample)
        assert (std::abs (processed.getSample (0, sample) - static_cast<float> (sample + 1) * -0.25f) < 0.0001f);
}

void testRemovedMaterialMeterIdentityIsZero()
{
    juce::AudioBuffer<float> original (1, 4);
    juce::AudioBuffer<float> processed (1, 4);

    for (int sample = 0; sample < 4; ++sample)
    {
        original.setSample (0, sample, static_cast<float> (sample + 1) * 0.1f);
        processed.setSample (0, sample, original.getSample (0, sample));
    }

    const auto stats = buffle::align::measureRemovedMaterial (original, processed, 1);
    assert (stats.amount == 0.0f);
    assert (stats.deltaRms == 0.0f);
    assert (stats.peakDelta == 0.0f);
}

void testRemovedMaterialMeterSilenceIsSafe()
{
    juce::AudioBuffer<float> original (2, 16);
    juce::AudioBuffer<float> processed (2, 16);
    original.clear();
    processed.clear();

    const auto stats = buffle::align::measureRemovedMaterial (original, processed, 2);
    assert (stats.amount == 0.0f);
    assert (stats.deltaRms == 0.0f);
    assert (stats.peakDelta == 0.0f);
}

void testRemovedMaterialMeterDetectsGainReducedTransient()
{
    juce::AudioBuffer<float> original (1, 8);
    juce::AudioBuffer<float> processed (1, 8);
    original.clear();
    processed.clear();
    original.setSample (0, 3, 1.0f);
    processed.setSample (0, 3, 0.25f);

    const auto stats = buffle::align::measureRemovedMaterial (original, processed, 1);
    assert (stats.amount > 0.0f);
    assert (std::abs (stats.peakDelta - 0.75f) < 0.001f);
}

void testRemovedMaterialMeterUsesRequestedChannels()
{
    juce::AudioBuffer<float> original (2, 8);
    juce::AudioBuffer<float> processed (2, 8);
    original.clear();
    processed.clear();
    original.setSample (0, 1, 1.0f);
    processed.setSample (0, 1, 0.5f);
    original.setSample (1, 1, 1.0f);
    processed.setSample (1, 1, -1.0f);

    const auto leftOnly = buffle::align::measureRemovedMaterial (original, processed, 1);
    const auto stereo = buffle::align::measureRemovedMaterial (original, processed, 2);

    assert (leftOnly.peakDelta < stereo.peakDelta);
    assert (stereo.peakDelta == 1.0f);
}

void testRemovedMaterialMeterCalibratesKnownGainReduction()
{
    juce::AudioBuffer<float> original (1, 32);
    juce::AudioBuffer<float> processed (1, 32);

    for (int sample = 0; sample < original.getNumSamples(); ++sample)
    {
        const auto value = sample % 2 == 0 ? 1.0f : -1.0f;
        original.setSample (0, sample, value);
        processed.setSample (0, sample, value * 0.5f);
    }

    const auto stats = buffle::align::measureRemovedMaterial (original, processed, 1);
    assert (std::abs (stats.amount - 0.5f) < 0.001f);
    assert (std::abs (stats.deltaRms - 0.5f) < 0.001f);
    assert (std::abs (stats.peakDelta - 0.5f) < 0.001f);
}

void testConsonantTamerFeedsRemovedMaterialMeter()
{
    juce::AudioBuffer<float> buffer (1, 4096);

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        buffer.setSample (0, sample, 0.06f * std::sin (static_cast<float> (sample) * 0.017f));

    buffer.setSample (0, 96, 1.0f);
    buffer.setSample (0, 97, -0.78f);
    const auto preTamer = buffer;

    buffle::align::ConsonantTamer tamer;
    tamer.prepare (44100.0, 1);
    tamer.process (buffer, nullptr, 1.0f, 0.2f);

    const auto stats = buffle::align::measureRemovedMaterial (preTamer, buffer, 1);
    assert (stats.amount > 0.01f);
    assert (stats.peakDelta > 0.05f);
}

void testNaturalnessGuardrailKeepsGentleCleanupSafe()
{
    const auto risk = buffle::align::assessNaturalnessRisk ({
        true, 12.0f, 0.02f, 0.62f, 0.58f, 0.35f, 1
    });

    assert (risk == buffle::align::NaturalnessRisk::safe);
    assert (std::string (buffle::align::getNaturalnessRiskLabel (risk)) == "Natural");
}

void testNaturalnessGuardrailAsksForDifferenceCheck()
{
    const auto risk = buffle::align::assessNaturalnessRisk ({
        true, 42.0f, 0.06f, 0.72f, 0.42f, 0.72f, 3
    });

    assert (risk == buffle::align::NaturalnessRisk::checkDifference);
}

void testNaturalnessGuardrailFlagsOverCleanedLooseRole()
{
    const auto risk = buffle::align::assessNaturalnessRisk ({
        true, 86.0f, 0.18f, 0.91f, 0.16f, 0.92f, 2
    });

    assert (risk == buffle::align::NaturalnessRisk::tooMuch);
    assert (std::string (buffle::align::getNaturalnessRiskLabel (risk)) == "Too Much");
}

void testTrustDiagnosticsPriority()
{
    using buffle::align::TrustState;

    assert (buffle::align::assessTrustState ({ false, true, 0.0f, 0.0f, 80.0f }) == TrustState::routeGuide);
    assert (buffle::align::assessTrustState ({ true, true, 0.0249f, 0.0f, 80.0f }) == TrustState::guideQuiet);
    assert (buffle::align::assessTrustState ({ true, true, 0.025f, 0.0249f, 80.0f }) == TrustState::dubQuiet);
    assert (buffle::align::assessTrustState ({ true, false, 0.025f, 0.025f, 80.0f }) == TrustState::listening);
}

void testTrustDiagnosticsThresholdsAndDirections()
{
    using buffle::align::TrustState;

    assert (buffle::align::assessTrustState ({ true, true, 0.025f, 0.025f, -0.05f }) == TrustState::locked);
    assert (buffle::align::assessTrustState ({ true, true, 0.025f, 0.025f, 0.05f }) == TrustState::locked);
    assert (buffle::align::assessTrustState ({ true, true, 0.025f, 0.025f, 0.0501f }) == TrustState::delayDub);
    assert (buffle::align::assessTrustState ({ true, true, 0.025f, 0.025f, -0.0501f }) == TrustState::advanceDub);
    assert (std::string (buffle::align::getTrustStateCode (TrustState::delayDub)) == "DUB_EARLY_SAFE_DELAY");
    assert (std::string (buffle::align::getTrustStateCode (TrustState::advanceDub)) == "DUB_LATE_SAFE_ADVANCE");
}

void testTrustDiagnosticsLabelsAndAdviceAreStable()
{
    using buffle::align::TrustState;

    assert (std::string (buffle::align::getTrustStateLabel (TrustState::routeGuide)) == "Route Guide sidechain");
    assert (std::string (buffle::align::getTrustStateAdvice (TrustState::routeGuide)).find ("sidechain") != std::string::npos);
    assert (std::string (buffle::align::getTrustStateAdvice (TrustState::guideQuiet)).find ("fake timing") != std::string::npos);
    assert (std::string (buffle::align::getTrustStateAdvice (TrustState::advanceDub)).find ("host latency") != std::string::npos);
}

void testStackRolePresetProfilesAreDistinct()
{
    const auto manual = buffle::align::getStackRoleSettings (buffle::align::StackRole::manual);
    const auto tight = buffle::align::getStackRoleSettings (buffle::align::StackRole::doubleTight);
    const auto choir = buffle::align::getStackRoleSettings (buffle::align::StackRole::choirNatural);
    const auto rap = buffle::align::getStackRoleSettings (buffle::align::StackRole::rapStack);
    const auto adr = buffle::align::getStackRoleSettings (buffle::align::StackRole::adrLoose);

    assert (tight.tightness > manual.tightness);
    assert (tight.stereoFocus > manual.stereoFocus);
    assert (choir.naturalness > tight.naturalness);
    assert (rap.consonantLevel > choir.consonantLevel);
    assert (adr.tightness < manual.tightness);
    assert (adr.naturalness > manual.naturalness);
}

void testAlignmentReportHidesUnreliableOffset()
{
    buffle::align::AlignmentReportInput input;
    input.guideFromSidechain = true;
    input.guideRms = 0.62f;
    input.dubRms = 0.58f;
    input.offsetConfidence = 0.12f;
    input.hasReliableOffset = false;
    input.estimatedOffsetMs = 42.0f;
    input.suggestedNudgeMs = -42.0f;

    const auto report = buffle::align::buildAlignmentReport (input);
    assert (report.find ("Phrase health: Listening for confidence") != std::string::npos);
    assert (report.find ("Trust reason: LISTENING_FOR_CONFIDENCE") != std::string::npos);
    assert (report.find ("Trust advice: Keep playback rolling") != std::string::npos);
    assert (report.find ("Estimated offset: unavailable") != std::string::npos);
    assert (report.find ("Suggested timing correction: unavailable") != std::string::npos);
    assert (report.find ("42.0 ms") == std::string::npos);
    assert (report.find ("Changed material: 0%") != std::string::npos);
    assert (report.find ("Naturalness risk: Natural") != std::string::npos);
}

void testAlignmentReportCapturesSafeNudgeAndRole()
{
    buffle::align::AlignmentReportInput input;
    input.guideFromSidechain = true;
    input.hasReliableOffset = true;
    input.guideRms = 0.64f;
    input.dubRms = 0.61f;
    input.offsetConfidence = 0.91f;
    input.estimatedOffsetMs = -18.4f;
    input.suggestedNudgeMs = 18.4f;
    input.currentNudgeMs = 18.4f;
    input.previewMode = 2;
    input.stackRole = 3;
    input.consonantLevel = 0.86f;
    input.removedMaterial = 0.34f;
    input.removedPeakDelta = 0.78f;
    input.consonantRemovedMaterial = 0.12f;
    input.consonantRemovedPeakDelta = 0.38f;
    input.naturalnessRisk = buffle::align::NaturalnessRisk::tooMuch;

    const auto report = buffle::align::buildAlignmentReport (input);
    assert (report.find ("Phrase health: Dub early - safe delay") != std::string::npos);
    assert (report.find ("Trust reason: DUB_EARLY_SAFE_DELAY") != std::string::npos);
    assert (report.find ("Estimated offset: -18.4 ms") != std::string::npos);
    assert (report.find ("Suggested timing correction: 18.4 ms") != std::string::npos);
    assert (report.find ("Preview mode: Difference") != std::string::npos);
    assert (report.find ("Stack role: Rap Stack") != std::string::npos);
    assert (report.find ("Consonant Tamer: 86%") != std::string::npos);
    assert (report.find ("Changed material: 34%") != std::string::npos);
    assert (report.find ("Peak changed material: 78%") != std::string::npos);
    assert (report.find ("Consonant removed: 12%") != std::string::npos);
    assert (report.find ("Peak consonant removed: 38%") != std::string::npos);
    assert (report.find ("Naturalness risk: Too Much") != std::string::npos);
}
}

int main()
{
    testSilenceEnvelopeIsZero();
    testImpulseProducesOnset();
    testShiftedClickOffset();
    testReverseShiftedClickOffset();
    testEqualClickOffsetIsZero();
    testSilenceOffsetHasLowConfidence();
    testManualNudgeZeroDelayIsIdentity();
    testManualNudgeDelaysImpulse();
    testBidirectionalNudgePlanUsesLatencyCentre();
    testBidirectionalNudgePlanAllowsNegativeAndPositiveMoves();
    testBidirectionalNudgePlanClampsAtLatencyEdges();
    testBidirectionalSuggestionInvertsOffset();
    testConsonantTamerAmountZeroIsIdentity();
    testConsonantTamerSilenceStaysSilent();
    testConsonantTamerReducesDubOnlyBurst();
    testConsonantTamerPreservesSustainedVowel();
    testConsonantTamerPreservesGuideMatchedAttack();
    testConsonantTamerMatchesSingleBlockAndChunkedProcessing();
    testPreviewModeOriginalRestoresInput();
    testPreviewModeAlignedKeepsProcessed();
    testPreviewModeDifferenceShowsChange();
    testPreviewModeConsonantRemovedShowsScopedChange();
    testRemovedMaterialMeterIdentityIsZero();
    testRemovedMaterialMeterSilenceIsSafe();
    testRemovedMaterialMeterDetectsGainReducedTransient();
    testRemovedMaterialMeterUsesRequestedChannels();
    testRemovedMaterialMeterCalibratesKnownGainReduction();
    testConsonantTamerFeedsRemovedMaterialMeter();
    testNaturalnessGuardrailKeepsGentleCleanupSafe();
    testNaturalnessGuardrailAsksForDifferenceCheck();
    testNaturalnessGuardrailFlagsOverCleanedLooseRole();
    testTrustDiagnosticsPriority();
    testTrustDiagnosticsThresholdsAndDirections();
    testTrustDiagnosticsLabelsAndAdviceAreStable();
    testStackRolePresetProfilesAreDistinct();
    testAlignmentReportHidesUnreliableOffset();
    testAlignmentReportCapturesSafeNudgeAndRole();

    std::cout << "Buffle Align DSP tests passed\n";
    return 0;
}
