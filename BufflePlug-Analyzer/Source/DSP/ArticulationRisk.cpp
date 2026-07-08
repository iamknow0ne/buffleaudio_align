#include "ArticulationRisk.h"

#include <algorithm>
#include <cmath>

namespace buffle::align
{
namespace
{
float clamp01 (float value) noexcept
{
    return std::clamp (value, 0.0f, 1.0f);
}
}

ArticulationRiskResult assessArticulationRisk (const ArticulationRiskInput& input) noexcept
{
    if (! input.hasReliableOffset)
        return {};

    const auto timingPressure = clamp01 ((std::abs (input.suggestedNudgeMs) - 8.0f) / 54.0f);
    const auto consonantPressure = std::max (clamp01 (input.consonantRemovedMaterial * 2.2f),
                                             clamp01 (input.consonantRemovedPeakDelta * 1.35f));
    const auto confidenceWeight = clamp01 (0.45f + input.offsetConfidence * 0.55f);
    const auto score = clamp01 ((timingPressure * 0.52f + consonantPressure * 0.48f) * confidenceWeight);

    if (score >= 0.62f)
        return { ArticulationRisk::collision, score };

    if (score >= 0.32f)
        return { ArticulationRisk::watch, score };

    return { ArticulationRisk::clean, score };
}

const char* getArticulationRiskLabel (ArticulationRisk risk) noexcept
{
    switch (risk)
    {
        case ArticulationRisk::clean: return "Clean";
        case ArticulationRisk::watch: return "Watch consonants";
        case ArticulationRisk::collision: return "Collision risk";
        case ArticulationRisk::listening:
        default: return "Listening";
    }
}

const char* getArticulationRiskAdvice (ArticulationRisk risk) noexcept
{
    switch (risk)
    {
        case ArticulationRisk::clean:
            return "Timing and tamer movement look controlled.";
        case ArticulationRisk::watch:
            return "A/B All Diff and Tamer before printing.";
        case ArticulationRisk::collision:
            return "Try less Tamer or a looser Stack Role before print.";
        case ArticulationRisk::listening:
        default:
            return "Route Guide and play the phrase to read consonant risk.";
    }
}

} // namespace buffle::align
