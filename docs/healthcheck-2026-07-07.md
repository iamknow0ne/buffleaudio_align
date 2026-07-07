# Buffle Audio Align Healthcheck - 2026-07-07

## Scope

This healthcheck covers the current post-`v0.3.0` preview polish slice:

- Experimental Consonant Tamer Lite DSP.
- Phrase-health UI strip and stateful workflow rail.
- Confidence-gated `Apply Nudge` availability.
- README, roadmap, landing, changelog, and release-doc synchronization.

## Verified

```bash
cmake --build build/cmake-debug --config Debug --parallel
ctest --test-dir build/cmake-debug --output-on-failure
```

Result: Debug Standalone, VST3, AU, shared code, DSP library, and DSP tests build successfully. `BuffleAlignDSPTests` passes.

The build still emits the known JUCE `juce_SimpleShapedText.h` warning and the existing VST3 ad-hoc signature/moduleinfo messages.

## Implemented In This Slice

- `ConsonantTamer` is now a standalone DSP module in `BufflePlug-Analyzer/Source/DSP/`.
- The processor applies Consonant Tamer after manual nudge and before preview output gain.
- The `Naturalness` parameter now influences the tamer depth/sensitivity.
- The editor labels the control as `Consonant Tamer` and `Preview Level`.
- The editor shows phrase health: route guide, quiet guide, quiet dub, listening, locked, or safe nudge ready.
- `Apply Nudge` is disabled until a reliable, positive suggested nudge exists.
- DSP tests now cover tamer identity, silence, Dub-only burst reduction, sustained vowel stability, and Guide-matched attack preservation.

## Still Open For V1

- Suggested nudge is positive-delay-first; full early/late compensation needs a dual-path or latency-aware design.
- Consonant Tamer Lite is not yet a full consonant collision detector.
- Removed-material audition and Original/Aligned/Difference A/B modes are not implemented.
- Capture/analyze/preview buttons remain workflow hints, not a complete phrase capture state machine.
- Installer packages are not Developer ID signed or notarized.
- The `.pkg` payload still needs AppleDouble `._*` cleanup before broad distribution.
- AU validation, pluginval/VST3 host validation, clean-account install smoke, and golden WAV regression tests are still missing.
