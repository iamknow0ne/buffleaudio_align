#pragma once

#include <vector>

namespace buffle::align
{
struct TimingOffsetResult
{
    int frameOffset = 0;
    float milliseconds = 0.0f;
    float confidence = 0.0f;
};

class TimingOffsetEstimator
{
public:
    TimingOffsetResult estimate (const std::vector<float>& guideEnvelope,
                                 const std::vector<float>& dubEnvelope,
                                 int maxFrameOffset,
                                 float hopMilliseconds) const;
};
}
