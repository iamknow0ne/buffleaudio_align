#include "WorkflowState.h"

#include <cmath>

namespace buffle::align
{

WorkflowStep assessWorkflowStep (const WorkflowStateInput& input) noexcept
{
    if (input.trustState == TrustState::routeGuide)
        return WorkflowStep::route;

    if (input.trustState == TrustState::guideQuiet
     || input.trustState == TrustState::dubQuiet
     || input.trustState == TrustState::listening
     || ! input.hasReliableOffset)
        return WorkflowStep::listen;

    if (input.intent == WorkflowIntent::printing)
        return WorkflowStep::print;

    if (input.intent == WorkflowIntent::previewing)
        return WorkflowStep::preview;

    if (input.phraseHealth == PhraseHealth::watchArticulation
     || input.phraseHealth == PhraseHealth::watchNaturalness
     || input.phraseHealth == PhraseHealth::changedMaterial
     || input.phraseHealth == PhraseHealth::tooMuch)
        return WorkflowStep::tame;

    if (input.intent == WorkflowIntent::checkingTiming
     || std::abs (input.suggestedNudgeMs) > 0.05f)
        return WorkflowStep::preview;

    return input.phraseHealth == PhraseHealth::clean ? WorkflowStep::print
                                                     : WorkflowStep::listen;
}

const char* getWorkflowIntentLabel (WorkflowIntent intent) noexcept
{
    switch (intent)
    {
        case WorkflowIntent::armedListen: return "Armed";
        case WorkflowIntent::checkingTiming: return "Checking";
        case WorkflowIntent::previewing: return "Previewing";
        case WorkflowIntent::printing: return "Printing";
        case WorkflowIntent::followSignal:
        default: return "Follow Signal";
    }
}

const char* getWorkflowStepLabel (WorkflowStep step) noexcept
{
    switch (step)
    {
        case WorkflowStep::route: return "Route";
        case WorkflowStep::listen: return "Listen";
        case WorkflowStep::preview: return "Preview";
        case WorkflowStep::tame: return "Tame";
        case WorkflowStep::print: return "Print";
        default: return "Listen";
    }
}

} // namespace buffle::align
