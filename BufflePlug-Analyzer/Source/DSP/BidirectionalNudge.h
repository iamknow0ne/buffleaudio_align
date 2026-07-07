#pragma once

namespace buffle::align
{

struct BidirectionalNudgePlan
{
    int hostLatencySamples = 0;
    int delaySamples = 0;
};

BidirectionalNudgePlan calculateBidirectionalNudgePlan (float nudgeMs,
                                                        double sampleRate,
                                                        float maxNudgeMs,
                                                        int maxDelaySamples) noexcept;

float suggestBidirectionalNudgeMs (float estimatedOffsetMs, float maxNudgeMs) noexcept;

} // namespace buffle::align
