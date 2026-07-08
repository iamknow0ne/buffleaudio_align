#include "PluginProcessor.h"

#include <cassert>
#include <cmath>
#include <iostream>

namespace
{
void testProcessorReportsDocumentedLatency()
{
    struct Case
    {
        double sampleRate;
        int expectedLatencySamples;
    };

    const Case cases[] {
        { 44100.0, 5292 },
        { 48000.0, 5760 },
        { 96000.0, 11520 }
    };

    for (const auto& c : cases)
    {
        BufflePlugAnalyzerAudioProcessor processor;
        processor.prepareToPlay (c.sampleRate, 512);
        assert (processor.getLatencySamples() == c.expectedLatencySamples);
    }
}

void testManualNudgeParameterContract()
{
    BufflePlugAnalyzerAudioProcessor processor;
    auto* parameter = processor.getValueTreeState().getParameter ("nudge");
    assert (parameter != nullptr);

    assert (std::abs (parameter->convertFrom0to1 (0.0f) - -120.0f) < 0.001f);
    assert (std::abs (parameter->convertFrom0to1 (0.5f) - 0.0f) < 0.001f);
    assert (std::abs (parameter->convertFrom0to1 (1.0f) - 120.0f) < 0.001f);
}

void testNegativeNudgeStateRoundTrip()
{
    BufflePlugAnalyzerAudioProcessor processor;
    auto* parameter = processor.getValueTreeState().getParameter ("nudge");
    assert (parameter != nullptr);

    parameter->beginChangeGesture();
    parameter->setValueNotifyingHost (parameter->convertTo0to1 (-37.5f));
    parameter->endChangeGesture();

    juce::MemoryBlock state;
    processor.getStateInformation (state);

    BufflePlugAnalyzerAudioProcessor restored;
    restored.setStateInformation (state.getData(), static_cast<int> (state.getSize()));

    auto* restoredValue = restored.getValueTreeState().getRawParameterValue ("nudge");
    assert (restoredValue != nullptr);
    assert (std::abs (restoredValue->load() - -37.5f) < 0.001f);
}
}

int main()
{
    testProcessorReportsDocumentedLatency();
    testManualNudgeParameterContract();
    testNegativeNudgeStateRoundTrip();

    std::cout << "Buffle Align processor latency contract tests passed\n";
    return 0;
}
