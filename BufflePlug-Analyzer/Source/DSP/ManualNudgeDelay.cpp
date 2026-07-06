#include "ManualNudgeDelay.h"

namespace buffle::align
{
void ManualNudgeDelay::prepare (int numChannels, int maxDelaySamples)
{
    delayBuffer.setSize (juce::jmax (1, numChannels), juce::jmax (1, maxDelaySamples));
    reset();
}

void ManualNudgeDelay::reset()
{
    delayBuffer.clear();
    writePosition = 0;
}

void ManualNudgeDelay::process (juce::AudioBuffer<float>& buffer, int channelsToDelay, int delaySamples)
{
    if (delaySamples <= 0 || delayBuffer.getNumSamples() <= 0)
        return;

    const auto boundedDelaySamples = juce::jlimit (0, delayBuffer.getNumSamples() - 1, delaySamples);
    const auto boundedChannels = juce::jmin (buffer.getNumChannels(),
                                             delayBuffer.getNumChannels(),
                                             juce::jmax (0, channelsToDelay));

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        const auto readPosition = (writePosition + delayBuffer.getNumSamples() - boundedDelaySamples)
                                % delayBuffer.getNumSamples();

        for (int channel = 0; channel < boundedChannels; ++channel)
        {
            auto* channelData = buffer.getWritePointer (channel);
            auto* delayData = delayBuffer.getWritePointer (channel);
            const auto input = channelData[sample];
            channelData[sample] = delayData[readPosition];
            delayData[writePosition] = input;
        }

        writePosition = (writePosition + 1) % delayBuffer.getNumSamples();
    }
}
}
