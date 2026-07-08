#include "BreathPreservationMask.h"

#include <algorithm>

namespace buffle::align
{
namespace
{
float saturate (float value) noexcept
{
    return std::clamp (value, 0.0f, 1.0f);
}
}

BreathMaskResult assessBreathProtection (const BreathMaskInput& input) noexcept
{
    const auto naturalness = saturate (input.naturalness);
    const auto dubTransient = std::max (0.0f, input.dubFast - input.dubSlow);
    const auto guideTransient = std::max (0.0f, input.guideFast - input.guideSlow);
    const auto sustainedBody = saturate (input.dubSlow * 9.0f);
    const auto softBody = 1.0f - saturate ((input.dubAbs - 0.055f) * 9.0f);
    const auto smoothness = 1.0f - saturate (dubTransient * 18.0f);
    const auto guideMatch = saturate (1.0f - std::max (0.0f, dubTransient - guideTransient) * 10.0f);
    const auto breathLike = sustainedBody * softBody * smoothness;
    const auto expressionProtection = saturate (0.35f + naturalness * 0.65f);
    const auto protection = saturate ((breathLike * 0.72f + guideMatch * 0.28f) * expressionProtection);

    return { protection, breathLike > 0.24f && dubTransient < 0.06f };
}

} // namespace buffle::align
