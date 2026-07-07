#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

#include <vector>

namespace buffle::align
{
class ConsonantTamer
{
public:
    void prepare (double sampleRate, int numChannels);
    void reset();

    void process (juce::AudioBuffer<float>& dubBuffer,
                  const juce::AudioBuffer<float>* guideBuffer,
                  float amount,
                  float naturalness);

private:
    struct ChannelState
    {
        float fast = 0.0f;
        float slow = 0.0f;
        float gain = 1.0f;
    };

    static float getMonoAbsAt (const juce::AudioBuffer<float>& buffer, int sample) noexcept;
    static float updateEnvelope (ChannelState& state, float value, float fastCoeff, float slowCoeff) noexcept;

    double currentSampleRate = 44100.0;
    float fastCoeff = 0.0f;
    float slowCoeff = 0.0f;
    float releaseCoeff = 0.0f;
    ChannelState guideState;
    std::vector<ChannelState> dubStates;
};
}
