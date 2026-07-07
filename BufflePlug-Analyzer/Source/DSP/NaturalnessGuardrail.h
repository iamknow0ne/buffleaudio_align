#pragma once

namespace buffle::align
{

enum class NaturalnessRisk
{
    safe = 0,
    checkDifference,
    tooMuch
};

struct NaturalnessGuardrailInput
{
    bool hasReliableOffset = false;
    float suggestedNudgeMs = 0.0f;
    float removedMaterial = 0.0f;
    float tightness = 0.0f;
    float naturalness = 0.0f;
    float consonantLevel = 0.0f;
    int stackRole = 0;
};

NaturalnessRisk assessNaturalnessRisk (const NaturalnessGuardrailInput& input) noexcept;
const char* getNaturalnessRiskLabel (NaturalnessRisk risk) noexcept;
const char* getNaturalnessRiskAdvice (NaturalnessRisk risk) noexcept;

} // namespace buffle::align
