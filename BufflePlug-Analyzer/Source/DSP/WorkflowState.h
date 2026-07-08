#pragma once

#include "PhraseHealth.h"
#include "TrustDiagnostics.h"

namespace buffle::align
{

enum class WorkflowIntent
{
    followSignal = 0,
    armedListen,
    checkingTiming,
    previewing,
    printing
};

enum class WorkflowStep
{
    route = 0,
    listen,
    preview,
    tame,
    print
};

struct WorkflowStateInput
{
    WorkflowIntent intent = WorkflowIntent::followSignal;
    TrustState trustState = TrustState::routeGuide;
    PhraseHealth phraseHealth = PhraseHealth::route;
    bool hasReliableOffset = false;
    float suggestedNudgeMs = 0.0f;
};

WorkflowStep assessWorkflowStep (const WorkflowStateInput& input) noexcept;
const char* getWorkflowIntentLabel (WorkflowIntent intent) noexcept;
const char* getWorkflowStepLabel (WorkflowStep step) noexcept;

} // namespace buffle::align
