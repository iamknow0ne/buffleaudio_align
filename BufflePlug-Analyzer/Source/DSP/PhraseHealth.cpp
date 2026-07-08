#include "PhraseHealth.h"

namespace buffle::align
{

PhraseHealth assessPhraseHealth (const PhraseHealthInput& input) noexcept
{
    switch (input.trustState)
    {
        case TrustState::routeGuide:
            return PhraseHealth::route;
        case TrustState::guideQuiet:
        case TrustState::dubQuiet:
        case TrustState::listening:
            return PhraseHealth::listen;
        case TrustState::delayDub:
        case TrustState::advanceDub:
            return PhraseHealth::safeNudge;
        case TrustState::locked:
            break;
    }

    if (input.naturalnessRisk == NaturalnessRisk::tooMuch)
        return PhraseHealth::tooMuch;

    if (input.articulationRisk == ArticulationRisk::collision
     || input.articulationRisk == ArticulationRisk::watch)
        return PhraseHealth::watchArticulation;

    if (input.naturalnessRisk == NaturalnessRisk::checkDifference)
        return PhraseHealth::watchNaturalness;

    if (input.removedMaterial > 0.08f || input.consonantRemovedMaterial > 0.06f)
        return PhraseHealth::changedMaterial;

    return PhraseHealth::clean;
}

const char* getPhraseHealthLabel (PhraseHealth health) noexcept
{
    switch (health)
    {
        case PhraseHealth::route: return "Route Guide";
        case PhraseHealth::listen: return "Listening";
        case PhraseHealth::safeNudge: return "Safe nudge ready";
        case PhraseHealth::clean: return "Print ready";
        case PhraseHealth::watchNaturalness: return "Check naturalness";
        case PhraseHealth::watchArticulation: return "Check articulation";
        case PhraseHealth::changedMaterial: return "Check changed material";
        case PhraseHealth::tooMuch: return "Too much cleanup";
        default: return "Listening";
    }
}

const char* getPhraseHealthAdvice (PhraseHealth health) noexcept
{
    switch (health)
    {
        case PhraseHealth::route:
            return "Route the Guide sidechain before trusting timing.";
        case PhraseHealth::listen:
            return "Keep playback rolling until Guide and Dub confidence locks.";
        case PhraseHealth::safeNudge:
            return "Apply the suggested nudge, then A/B Aligned against Diff.";
        case PhraseHealth::clean:
            return "Timing and cleanup are inside the current guardrails.";
        case PhraseHealth::watchNaturalness:
            return "A/B Diff before printing; the move may feel over-tight.";
        case PhraseHealth::watchArticulation:
            return "A/B Tamer and Diff before printing consonant-heavy material.";
        case PhraseHealth::changedMaterial:
            return "Review Diff because the preview changed enough material to hear.";
        case PhraseHealth::tooMuch:
            return "Loosen the role, lower Tamer, or reduce nudge before printing.";
        default:
            return "Keep listening until the phrase read is trustworthy.";
    }
}

} // namespace buffle::align
