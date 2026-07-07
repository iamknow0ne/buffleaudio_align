#include "NaturalnessGuardrail.h"

#include <algorithm>
#include <cmath>

namespace buffle::align
{
namespace
{
constexpr auto choirNaturalRole = 2;
constexpr auto adrLooseRole = 4;

bool isLooseRole (int stackRole) noexcept
{
    return stackRole == choirNaturalRole || stackRole == adrLooseRole;
}

} // namespace

NaturalnessRisk assessNaturalnessRisk (const NaturalnessGuardrailInput& input) noexcept
{
    const auto changed = std::clamp (input.removedMaterial, 0.0f, 1.0f);
    const auto tightness = std::clamp (input.tightness, 0.0f, 1.0f);
    const auto naturalness = std::clamp (input.naturalness, 0.0f, 1.0f);
    const auto tamer = std::clamp (input.consonantLevel, 0.0f, 1.0f);
    const auto nudge = input.hasReliableOffset ? std::abs (input.suggestedNudgeMs) : 0.0f;
    auto score = 0.0f;

    if (changed > 0.04f)
        score += 0.75f;

    if (changed > 0.12f)
        score += 1.0f;

    if (nudge > 35.0f)
        score += 0.55f;

    if (nudge > 75.0f)
        score += 0.75f;

    if (tightness > 0.84f)
        score += 0.55f;

    if (naturalness < 0.24f)
        score += 0.65f;

    if (tamer > 0.82f)
        score += 0.55f;

    if (isLooseRole (input.stackRole) && (tightness > 0.76f || naturalness < 0.36f || changed > 0.08f))
        score += 0.7f;

    if (score >= 2.2f)
        return NaturalnessRisk::tooMuch;

    if (score >= 0.75f)
        return NaturalnessRisk::checkDifference;

    return NaturalnessRisk::safe;
}

const char* getNaturalnessRiskLabel (NaturalnessRisk risk) noexcept
{
    switch (risk)
    {
        case NaturalnessRisk::checkDifference: return "Check Diff";
        case NaturalnessRisk::tooMuch: return "Too Much";
        case NaturalnessRisk::safe:
        default: return "Natural";
    }
}

const char* getNaturalnessRiskAdvice (NaturalnessRisk risk) noexcept
{
    switch (risk)
    {
        case NaturalnessRisk::checkDifference:
            return "A/B Difference before printing this move.";
        case NaturalnessRisk::tooMuch:
            return "Try lower Tamer, looser Tightness, or a more natural role.";
        case NaturalnessRisk::safe:
        default:
            return "Cleanup is inside the current naturalness guardrail.";
    }
}

} // namespace buffle::align
