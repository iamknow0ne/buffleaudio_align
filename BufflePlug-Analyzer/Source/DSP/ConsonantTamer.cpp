#include "ConsonantTamer.h"

#include <cmath>

namespace buffle::align
{
namespace
{
float coefficientForMs (double sampleRate, float milliseconds)
{
    const auto samples = juce::jmax (1.0, sampleRate * static_cast<double> (milliseconds) / 1000.0);
    return static_cast<float> (std::exp (-1.0 / samples));
}

}

void ConsonantTamer::prepare (double sampleRate, int numChannels)
{
    currentSampleRate = sampleRate > 0.0 ? sampleRate : 44100.0;
    fastCoeff = coefficientForMs (currentSampleRate, 1.8f);
    slowCoeff = coefficientForMs (currentSampleRate, 38.0f);
    releaseCoeff = coefficientForMs (currentSampleRate, 24.0f);
    dubStates.resize (static_cast<size_t> (juce::jmax (1, numChannels)));
    reset();
}

void ConsonantTamer::reset()
{
    guideState = {};

    for (auto& state : dubStates)
        state = {};
}

void ConsonantTamer::process (juce::AudioBuffer<float>& dubBuffer,
                              const juce::AudioBuffer<float>* guideBuffer,
                              float amount,
                              float naturalness)
{
    const auto boundedAmount = juce::jlimit (0.0f, 1.0f, amount);

    if (boundedAmount <= 0.001f || dubBuffer.getNumSamples() == 0)
        return;

    if (dubStates.size() < static_cast<size_t> (dubBuffer.getNumChannels()))
        dubStates.resize (static_cast<size_t> (dubBuffer.getNumChannels()));

    const auto boundedNaturalness = juce::jlimit (0.0f, 1.0f, naturalness);
    const auto maxReduction = juce::jmap (boundedNaturalness, 0.62f, 0.32f) * boundedAmount;
    const auto sensitivity = juce::jmap (boundedNaturalness, 9.0f, 5.0f);
    const auto floor = juce::jmap (boundedNaturalness, 0.006f, 0.018f);

    for (int sample = 0; sample < dubBuffer.getNumSamples(); ++sample)
    {
        auto guideTransient = 0.0f;

        if (guideBuffer != nullptr && guideBuffer->getNumSamples() > sample)
        {
            const auto guideAbs = getMonoAbsAt (*guideBuffer, sample);
            guideTransient = juce::jmax (updateEnvelope (guideState, guideAbs, fastCoeff, slowCoeff),
                                         guideAbs - guideState.slow * 1.6f);
        }

        for (int channel = 0; channel < dubBuffer.getNumChannels(); ++channel)
        {
            auto& state = dubStates[static_cast<size_t> (channel)];
            auto* samples = dubBuffer.getWritePointer (channel);
            const auto dubAbs = std::abs (samples[sample]);
            const auto dubTransient = juce::jmax (updateEnvelope (state, dubAbs, fastCoeff, slowCoeff),
                                                  dubAbs - state.slow * 1.6f);
            const auto unmaskedTransient = juce::jmax (0.0f, dubTransient - guideTransient * 0.9f - floor);
            const auto reduction = maxReduction * juce::jlimit (0.0f, 1.0f, unmaskedTransient * sensitivity);
            const auto targetGain = 1.0f - reduction;

            state.gain = targetGain < state.gain
                ? targetGain
                : releaseCoeff * state.gain + (1.0f - releaseCoeff) * targetGain;

            samples[sample] *= state.gain;
        }
    }
}

float ConsonantTamer::getMonoAbsAt (const juce::AudioBuffer<float>& buffer, int sample) noexcept
{
    if (sample < 0 || sample >= buffer.getNumSamples() || buffer.getNumChannels() == 0)
        return 0.0f;

    auto sum = 0.0f;
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        sum += std::abs (buffer.getSample (channel, sample));

    return sum / static_cast<float> (buffer.getNumChannels());
}

float ConsonantTamer::updateEnvelope (ChannelState& state, float value, float fast, float slow) noexcept
{
    state.fast = fast * state.fast + (1.0f - fast) * value;
    state.slow = slow * state.slow + (1.0f - slow) * value;
    return juce::jmax (0.0f, state.fast - state.slow);
}
}
