# Buffle Audio Align Healthcheck - 2026-07-07 Copy Report

## Scope

This healthcheck covers the post-`v0.3.0` clipboard report and first-run clarity slice:

- `AlignmentReport` DSP-side formatter.
- In-editor `Copy Report` clipboard action.
- Dynamic Apply Nudge button labels.
- Quiet-lane hints for missing Guide/Dub signal.
- Documentation and landing truth pass for positive-delay-first nudge and rough installer status.

## Verified

```bash
cmake --build build/cmake-debug --target BuffleAlignDSPTests --config Debug --parallel
ctest --test-dir build/cmake-debug --output-on-failure
cmake --build build/cmake-debug --config Debug --parallel
```

Result: Debug Standalone, VST3, AU, shared code, DSP library, and DSP tests build successfully. `BuffleAlignDSPTests` passes.

The build still emits the known JUCE `juce_SimpleShapedText.h` warning and existing VST3 ad-hoc signature/moduleinfo messages.

## Implemented In This Slice

- `AlignmentReport` builds a plain-text handoff summary from phrase health, Guide/Dub levels, confidence, offset, suggested nudge, current nudge, preview mode, stack role, and current controls.
- `Copy Report` places that summary on the clipboard for session/tester handoff.
- Tests cover unreliable report states and safe-nudge report states.
- The workflow buttons now read `Arm Listen`, `Check Timing`, and `Preview Align`.
- Apply Nudge now says `Waiting...`, `No Nudge`, or `Apply +N ms` based on the current confidence state.
- Empty Guide/Dub lanes now show quiet-signal hints instead of looking broken.

## Still Open For V1

- Suggested nudge is still positive-delay-first; bidirectional early/late compensation remains open.
- Copy Report is clipboard-only and does not yet include removed-material metering.
- Capture/analyze/preview buttons remain workflow hints, not a complete phrase capture state machine.
- Dedicated removed-material metering is still missing.
- The `.pkg` preview artifact still needs AppleDouble payload cleanup before signing/notarization.
