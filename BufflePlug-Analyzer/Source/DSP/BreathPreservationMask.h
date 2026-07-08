#pragma once

namespace buffle::align
{

struct BreathMaskInput
{
    float dubAbs = 0.0f;
    float guideAbs = 0.0f;
    float dubFast = 0.0f;
    float dubSlow = 0.0f;
    float guideFast = 0.0f;
    float guideSlow = 0.0f;
    float naturalness = 0.0f;
};

struct BreathMaskResult
{
    float protection = 0.0f;
    bool likelyBreath = false;
};

BreathMaskResult assessBreathProtection (const BreathMaskInput& input) noexcept;

} // namespace buffle::align
