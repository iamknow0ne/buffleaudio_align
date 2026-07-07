#include "AlignmentReport.h"
#include "PreviewModeMixer.h"
#include "StackRolePreset.h"

#include <algorithm>
#include <iomanip>
#include <sstream>

namespace buffle::align
{
namespace
{
constexpr auto usableSignalFloor = 0.025f;

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
        default: return "Aligned";
    }
}

} // namespace

const char* getAlignmentReportPhraseHealth (const AlignmentReportInput& input) noexcept
{
    if (! input.guideFromSidechain)
        return "Route Guide sidechain";

    if (input.guideRms < usableSignalFloor)
        return "Guide too quiet";

    if (input.dubRms < usableSignalFloor)
        return "Dub too quiet";

    if (! input.hasReliableOffset)
        return "Listening for confidence";

    if (input.suggestedNudgeMs <= 0.05f)
        return "Locked - no delay needed";

    return "Safe nudge ready";
}

std::string buildAlignmentReport (const AlignmentReportInput& input)
{
    const auto role = static_cast<StackRole> (std::clamp (input.stackRole, 0, 4));

    std::ostringstream report;
    report << std::fixed << std::setprecision (1);
    report << "Buffle Audio Align Report\n";
    report << "Phrase health: " << getAlignmentReportPhraseHealth (input) << "\n";
    report << "Guide source: " << (input.guideFromSidechain ? "Sidechain" : "Missing / fallback") << "\n";
    report << "Guide level: " << asPercent (input.guideRms) << "%\n";
    report << "Dub level: " << asPercent (input.dubRms) << "%\n";
    report << "Confidence: " << asPercent (input.offsetConfidence) << "%\n";

    if (input.hasReliableOffset)
    {
        report << "Estimated offset: " << input.estimatedOffsetMs << " ms\n";
        report << "Suggested safe nudge: " << input.suggestedNudgeMs << " ms\n";
    }
    else
    {
        report << "Estimated offset: unavailable\n";
        report << "Suggested safe nudge: unavailable\n";
    }

    report << "Current nudge: " << input.currentNudgeMs << " ms\n";
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
