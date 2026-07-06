#include "TimingOffsetEstimator.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace buffle::align
{
namespace
{
float scoreLag (const std::vector<float>& guide, const std::vector<float>& dub, int lag)
{
    double dot = 0.0;
    double guideEnergy = 0.0;
    double dubEnergy = 0.0;
    int count = 0;

    for (int i = 0; i < static_cast<int> (guide.size()); ++i)
    {
        const auto dubIndex = i + lag;
        if (dubIndex < 0 || dubIndex >= static_cast<int> (dub.size()))
            continue;

        const auto guideValue = static_cast<double> (guide[static_cast<size_t> (i)]);
        const auto dubValue = static_cast<double> (dub[static_cast<size_t> (dubIndex)]);
        dot += guideValue * dubValue;
        guideEnergy += guideValue * guideValue;
        dubEnergy += dubValue * dubValue;
        ++count;
    }

    if (count == 0 || guideEnergy <= 1.0e-12 || dubEnergy <= 1.0e-12)
        return 0.0f;

    return static_cast<float> (dot / std::sqrt (guideEnergy * dubEnergy));
}
}

TimingOffsetResult TimingOffsetEstimator::estimate (const std::vector<float>& guideEnvelope,
                                                    const std::vector<float>& dubEnvelope,
                                                    int maxFrameOffset,
                                                    float hopMilliseconds) const
{
    TimingOffsetResult result;

    if (guideEnvelope.empty() || dubEnvelope.empty() || maxFrameOffset <= 0 || hopMilliseconds <= 0.0f)
        return result;

    const auto boundedMaxOffset = std::min (maxFrameOffset,
                                            static_cast<int> (std::max (guideEnvelope.size(), dubEnvelope.size())) - 1);
    auto bestScore = -std::numeric_limits<float>::infinity();

    for (int lag = -boundedMaxOffset; lag <= boundedMaxOffset; ++lag)
    {
        const auto score = scoreLag (guideEnvelope, dubEnvelope, lag);
        if (score > bestScore)
        {
            bestScore = score;
            result.frameOffset = lag;
        }
    }

    result.confidence = std::clamp (bestScore, 0.0f, 1.0f);
    result.milliseconds = static_cast<float> (result.frameOffset) * hopMilliseconds;
    return result;
}
}
