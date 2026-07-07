#pragma once

namespace buffle::align
{

enum class TrustState
{
    routeGuide = 0,
    guideQuiet,
    dubQuiet,
    listening,
    locked,
    delayDub,
    advanceDub
};

struct TrustDiagnosticInput
{
    bool guideFromSidechain = false;
    bool hasReliableOffset = false;
    float guideRms = 0.0f;
    float dubRms = 0.0f;
    float suggestedNudgeMs = 0.0f;
};

TrustState assessTrustState (const TrustDiagnosticInput& input) noexcept;
const char* getTrustStateCode (TrustState state) noexcept;
const char* getTrustStateLabel (TrustState state) noexcept;
const char* getTrustStateAdvice (TrustState state) noexcept;

} // namespace buffle::align
