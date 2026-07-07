#include "AlignmentReport.h"
#include "PreviewModeMixer.h"
#include "StackRolePreset.h"

#include <algorithm>
#include <iomanip>
#include <sstream>

namespace buffle::align
{
int asPercent (float value) noexcept
{
    return static_cast<int> (std::clamp (value, 0.0f, 1.0f) * 100.0f + 0.5f);
}

const char* previewModeName (int mode) noexcept
{
    switch (mode)
    {
        case 0: return "Original";
        case 2: return "Difference";
        case 3: return "Tamer Removed";
        default: return "Aligned";
    }
}

const char* getAlignmentReportPhraseHealth (const AlignmentReportInput& input) noexcept
{
    const auto state = input.trustState == TrustState::routeGuide
        ? assessTrustState ({ input.guideFromSidechain,
                              input.hasReliableOffset,
                              input.guideRms,
                              input.dubRms,
                              input.suggestedNudgeMs })
        : input.trustState;

    return getTrustStateLabel (state);
}

std::string buildAlignmentReport (const AlignmentReportInput& input)
{
    const auto role = static_cast<StackRole> (std::clamp (input.stackRole, 0, 4));

    std::ostringstream report;
    report << std::fixed << std::setprecision (1);
    report << "Buffle Audio Align Report\n";
    const auto trustState = input.trustState == TrustState::routeGuide
        ? assessTrustState ({ input.guideFromSidechain,
                              input.hasReliableOffset,
                              input.guideRms,
                              input.dubRms,
                              input.suggestedNudgeMs })
        : input.trustState;

    report << "Phrase health: " << getTrustStateLabel (trustState) << "\n";
    report << "Trust reason: " << getTrustStateCode (trustState) << "\n";
    report << "Trust advice: " << getTrustStateAdvice (trustState) << "\n";
    report << "Guide source: " << (input.guideFromSidechain ? "Sidechain" : "Missing / fallback") << "\n";
    report << "Guide level: " << asPercent (input.guideRms) << "%\n";
    report << "Dub level: " << asPercent (input.dubRms) << "%\n";
    report << "Confidence: " << asPercent (input.offsetConfidence) << "%\n";

    if (input.hasReliableOffset)
    {
        report << "Estimated offset: " << input.estimatedOffsetMs << " ms\n";
        report << "Suggested timing correction: " << input.suggestedNudgeMs << " ms\n";
    }
    else
    {
        report << "Estimated offset: unavailable\n";
        report << "Suggested timing correction: unavailable\n";
    }

    report << "Current nudge: " << input.currentNudgeMs << " ms\n";
    report << "Changed material: " << asPercent (input.removedMaterial) << "%\n";
    report << "Peak changed material: " << asPercent (input.removedPeakDelta) << "%\n";
    report << "Consonant removed: " << asPercent (input.consonantRemovedMaterial) << "%\n";
    report << "Peak consonant removed: " << asPercent (input.consonantRemovedPeakDelta) << "%\n";
    report << "Naturalness risk: " << getNaturalnessRiskLabel (input.naturalnessRisk)
           << " - " << getNaturalnessRiskAdvice (input.naturalnessRisk) << "\n";
    report << "Preview mode: " << previewModeName (input.previewMode) << "\n";
    report << "Stack role: " << getStackRoleName (role) << "\n";
    report << "Tightness: " << asPercent (input.tightness) << "%\n";
    report << "Naturalness: " << asPercent (input.naturalness) << "%\n";
    report << "Consonant Tamer: " << asPercent (input.consonantLevel) << "%\n";
    report << "Guide Blend: " << asPercent (input.guideBlend) << "%\n";
    report << "Stereo Focus: " << asPercent (input.stereoFocus) << "%\n";
    return report.str();
}

} // namespace buffle::align
