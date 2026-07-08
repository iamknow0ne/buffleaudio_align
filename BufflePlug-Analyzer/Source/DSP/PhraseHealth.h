#pragma once

#include "ArticulationRisk.h"
#include "NaturalnessGuardrail.h"
#include "TrustDiagnostics.h"

namespace buffle::align
{

enum class PhraseHealth
{
    route = 0,
    listen,
    safeNudge,
    clean,
    watchNaturalness,
    watchArticulation,
    changedMaterial,
    tooMuch
};

struct PhraseHealthInput
{
    TrustState trustState = TrustState::routeGuide;
    NaturalnessRisk naturalnessRisk = NaturalnessRisk::safe;
    ArticulationRisk articulationRisk = ArticulationRisk::listening;
    float removedMaterial = 0.0f;
    float consonantRemovedMaterial = 0.0f;
};

PhraseHealth assessPhraseHealth (const PhraseHealthInput& input) noexcept;
const char* getPhraseHealthLabel (PhraseHealth health) noexcept;
const char* getPhraseHealthAdvice (PhraseHealth health) noexcept;

} // namespace buffle::align
