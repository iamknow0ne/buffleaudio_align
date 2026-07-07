# Buffle Audio Align Healthcheck - 2026-07-07 A/B Preview

## Scope

This healthcheck covers the post-`v0.3.0` A/B preview slice:

- Persistent `Preview Mode` parameter.
- Original / Aligned / Difference monitor buttons.
- Difference rendering for changed/removed material audition.
- Refreshed standalone screenshot for README and landing.

## Verified

```bash
cmake --build build/cmake-debug --config Debug --parallel
ctest --test-dir build/cmake-debug --output-on-failure
```

Result: Debug Standalone, VST3, AU, shared code, DSP library, and DSP tests build successfully. `BuffleAlignDSPTests` passes.

The build still emits the known JUCE `juce_SimpleShapedText.h` warning and the existing VST3 ad-hoc signature/moduleinfo messages.

## Implemented In This Slice

- `PreviewModeMixer` is now a standalone DSP utility.
- `Preview Mode` is persisted in APVTS with `Original`, `Aligned`, and `Difference` choices.
- The processor keeps a prepared original-Dub scratch buffer, avoiding per-block `makeCopyOf` allocation for the A/B path.
- `Original` restores the dry Dub monitor path.
- `Aligned` keeps the current nudge and Consonant Tamer processing.
- `Difference` outputs the processed path minus the original Dub for changed-material audition.
- The editor exposes a compact Original / Aligned / Diff segmented control in the workflow rail.
- DSP tests now cover Original restore, Aligned passthrough, and Difference subtraction.

## Still Open For V1

- Difference mode is a broad processed-minus-original audition, not yet per-feature removed-material solo.
- No visual removed-material meter yet.
- Suggested nudge is still positive-delay-first.
- Capture/analyze/preview buttons remain workflow hints, not a complete phrase capture state machine.
- Installer signing/notarization, package payload cleanup, DAW host validation, and golden WAV regression tests are still missing.
