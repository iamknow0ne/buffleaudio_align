# Buffle Audio Align Healthcheck - 2026-07-07 Stack Roles

## Scope

This healthcheck covers the post-`v0.3.0` Stack Role preset slice:

- Persistent `Stack Role` parameter.
- User-facing `Manual`, `Tight`, `Natural`, `Rap`, and `ADR` selector.
- Role presets for `Double Tight`, `Choir Natural`, `Rap Stack`, and `ADR Loose`.
- README, roadmap, landing, changelog, release notes, and screenshot synchronization.

## Verified

```bash
cmake --build build/cmake-debug --target BuffleAlignDSPTests --config Debug --parallel
ctest --test-dir build/cmake-debug --output-on-failure
cmake --build build/cmake-debug --config Debug --parallel
```

Result: Debug Standalone, VST3, AU, shared code, DSP library, and DSP tests build successfully. `BuffleAlignDSPTests` passes.

The build still emits the known JUCE `juce_SimpleShapedText.h` warning and the existing VST3 ad-hoc signature/moduleinfo messages.

## Implemented In This Slice

- `StackRolePreset` is now a standalone DSP profile definition.
- `Stack Role` is persisted in APVTS with Manual, Double Tight, Choir Natural, Rap Stack, and ADR Loose choices.
- The editor exposes a compact Stack Role selector in the Control Room header.
- Selecting a role applies Tightness, Naturalness, Consonant Tamer, Guide Blend, and Stereo Focus.
- Role presets do not change Nudge, Preview Mode, or Preview Level.
- Manual edits to role-owned sliders return the role to Manual.
- DSP tests cover distinct preset profile behavior.

## Still Open For V1

- Tightness, Guide Blend, and Stereo Focus are not yet all deep DSP behaviors; today the most audible preset movement comes from Naturalness and Consonant Tamer.
- Bidirectional nudge was added in a later slice; this Stack Role slice did not change nudge behavior.
- Capture/analyze/preview buttons remain workflow hints, not a complete phrase capture state machine.
- Dedicated per-feature removed-material metering is still missing; a later slice added the initial broad changed-material meter.
- Installer signing/notarization, package payload cleanup, DAW host validation, and golden WAV regression tests are still missing.
