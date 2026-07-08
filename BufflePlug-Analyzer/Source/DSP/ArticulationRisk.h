#pragma once

namespace buffle::align
{

enum class ArticulationRisk
{
    listening = 0,
    clean,
    watch,
    collision
};

struct ArticulationRiskInput
{
    bool hasReliableOffset = false;
    float suggestedNudgeMs = 0.0f;
    float offsetConfidence = 0.0f;
    float consonantRemovedMaterial = 0.0f;
    float consonantRemovedPeakDelta = 0.0f;
};

struct ArticulationRiskResult
{
    ArticulationRisk risk = ArticulationRisk::listening;
    float score = 0.0f;
};

ArticulationRiskResult assessArticulationRisk (const ArticulationRiskInput& input) noexcept;
const char* getArticulationRiskLabel (ArticulationRisk risk) noexcept;
const char* getArticulationRiskAdvice (ArticulationRisk risk) noexcept;

} // namespace buffle::align
