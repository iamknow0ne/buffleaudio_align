#pragma once

#include <string>

#include "NaturalnessGuardrail.h"
#include "TrustDiagnostics.h"

namespace buffle::align
{

struct AlignmentReportInput
{
    bool guideFromSidechain = false;
    bool hasReliableOffset = false;
    float guideRms = 0.0f;
    float dubRms = 0.0f;
    float offsetConfidence = 0.0f;
    float estimatedOffsetMs = 0.0f;
    float suggestedNudgeMs = 0.0f;
    float currentNudgeMs = 0.0f;
    float removedMaterial = 0.0f;
    float removedPeakDelta = 0.0f;
    int previewMode = 1;
    int stackRole = 0;
    float tightness = 0.0f;
    float naturalness = 0.0f;
    float consonantLevel = 0.0f;
    float guideBlend = 0.0f;
    float stereoFocus = 0.0f;
    NaturalnessRisk naturalnessRisk = NaturalnessRisk::safe;
    TrustState trustState = TrustState::routeGuide;
};

const char* getAlignmentReportPhraseHealth (const AlignmentReportInput& input) noexcept;
std::string buildAlignmentReport (const AlignmentReportInput& input);

} // namespace buffle::align
