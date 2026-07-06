#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

namespace buffle::align
{
class ManualNudgeDelay
{
public:
    void prepare (int numChannels, int maxDelaySamples);
    void reset();

    void process (juce::AudioBuffer<float>& buffer, int channelsToDelay, int delaySamples);

    int getMaxDelaySamples() const noexcept { return delayBuffer.getNumSamples(); }

private:
    int writePosition = 0;
    juce::AudioBuffer<float> delayBuffer;
};
}
