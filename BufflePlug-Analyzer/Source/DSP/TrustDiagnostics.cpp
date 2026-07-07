#include "TrustDiagnostics.h"

#include <cmath>

namespace buffle::align
{
namespace
{
constexpr auto usableSignalFloor = 0.025f;
constexpr auto actionableNudgeFloorMs = 0.05f;
}

TrustState assessTrustState (const TrustDiagnosticInput& input) noexcept
{
    if (! input.guideFromSidechain)
        return TrustState::routeGuide;

    if (input.guideRms < usableSignalFloor)
        return TrustState::guideQuiet;

    if (input.dubRms < usableSignalFloor)
        return TrustState::dubQuiet;

    if (! input.hasReliableOffset)
        return TrustState::listening;

    if (std::abs (input.suggestedNudgeMs) <= actionableNudgeFloorMs)
        return TrustState::locked;

    return input.suggestedNudgeMs > 0.0f ? TrustState::delayDub
                                         : TrustState::advanceDub;
}

const char* getTrustStateCode (TrustState state) noexcept
{
    switch (state)
    {
        case TrustState::routeGuide: return "ROUTE_GUIDE";
        case TrustState::guideQuiet: return "GUIDE_TOO_QUIET";
        case TrustState::dubQuiet: return "DUB_TOO_QUIET";
        case TrustState::listening: return "LISTENING_FOR_CONFIDENCE";
        case TrustState::locked: return "LOCKED_NO_NUDGE";
        case TrustState::delayDub: return "DUB_EARLY_SAFE_DELAY";
        case TrustState::advanceDub: return "DUB_LATE_SAFE_ADVANCE";
        default: return "LISTENING_FOR_CONFIDENCE";
    }
}

const char* getTrustStateLabel (TrustState state) noexcept
{
    switch (state)
    {
        case TrustState::routeGuide: return "Route Guide sidechain";
        case TrustState::guideQuiet: return "Guide too quiet";
        case TrustState::dubQuiet: return "Dub too quiet";
        case TrustState::listening: return "Listening for confidence";
        case TrustState::locked: return "Locked - no timing nudge needed";
        case TrustState::delayDub: return "Dub early - safe delay";
        case TrustState::advanceDub: return "Dub late - safe advance";
        default: return "Listening for confidence";
    }
}

const char* getTrustStateAdvice (TrustState state) noexcept
{
    switch (state)
    {
        case TrustState::routeGuide:
            return "Route the Guide into the sidechain before trusting timing.";
        case TrustState::guideQuiet:
            return "Raise or unmute the Guide; Align is refusing fake timing certainty.";
        case TrustState::dubQuiet:
            return "Raise or unmute the Dub before nudge, tamer, or report decisions.";
        case TrustState::listening:
            return "Keep playback rolling until the Guide/Dub timing read locks.";
        case TrustState::locked:
            return "Timing is already inside the current nudge threshold.";
        case TrustState::delayDub:
            return "A confidence-gated delay is available for the Dub.";
        case TrustState::advanceDub:
            return "A confidence-gated advance is available through host latency compensation.";
        default:
            return "Keep playback rolling until the timing read is trustworthy.";
    }
}

} // namespace buffle::align
