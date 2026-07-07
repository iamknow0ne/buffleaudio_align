#include "BidirectionalNudge.h"

#include <algorithm>
#include <cmath>

namespace buffle::align
{

BidirectionalNudgePlan calculateBidirectionalNudgePlan (float nudgeMs,
                                                        double sampleRate,
                                                        float maxNudgeMs,
                                                        int maxDelaySamples) noexcept
{
    if (sampleRate <= 0.0 || maxNudgeMs <= 0.0f || maxDelaySamples <= 0)
        return {};

    const auto hostLatencySamples = static_cast<int> (std::round (sampleRate * static_cast<double> (maxNudgeMs) / 1000.0));
    const auto signedNudgeMs = std::clamp (nudgeMs, -maxNudgeMs, maxNudgeMs);
    const auto signedNudgeSamples = static_cast<int> (std::round (sampleRate * static_cast<double> (signedNudgeMs) / 1000.0));
    const auto delaySamples = std::clamp (hostLatencySamples + signedNudgeSamples, 0, std::max (0, maxDelaySamples - 1));

    return { hostLatencySamples, delaySamples };
}

float suggestBidirectionalNudgeMs (float estimatedOffsetMs, float maxNudgeMs) noexcept
{
    return std::clamp (-estimatedOffsetMs, -maxNudgeMs, maxNudgeMs);
}

} // namespace buffle::align
