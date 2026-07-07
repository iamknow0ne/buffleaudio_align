# Changelog

## Unreleased

- Added clipboard `Copy Report` handoff summaries for phrase health, confidence, offset, suggested nudge, changed-material amount, preview mode, stack role, and current controls.
- Added `AlignmentReport` formatter and CTest coverage for unreliable and safe-nudge report states.
- Added initial `RemovedMaterialMeter` DSP helper for processed-vs-original changed material amount, RMS delta, and peak delta, with CTest coverage.
- Surfaced a compact `CHANGED` preview-delta meter in the editor and Copy Report.
- Improved first-run UI clarity with `Arm Listen`, `Check Timing`, `Preview Align`, dynamic Apply Nudge labels, and quiet-lane hints.
- Added user-facing `Stack Role` presets for `Double Tight`, `Choir Natural`, `Rap Stack`, and `ADR Loose`.
- Added `StackRolePreset` DSP profile definitions and CTest coverage for distinct role behavior.
- Added persistent `Preview Mode` choices for Original, Aligned, and Difference monitoring.
- Added `PreviewModeMixer` DSP and tests for Original restore, Aligned passthrough, and Difference audition rendering.
- Added an in-editor Original / Aligned / Diff control and refreshed the standalone screenshot.
- Added experimental `ConsonantTamer` DSP for reducing unmatched Dub consonant bursts while preserving sustained vowels and Guide-matched attacks.
- Added CTest coverage for Consonant Tamer identity, silence, Dub-only burst reduction, sustained vowel stability, and Guide-matched attack preservation.
- Added a phrase-health strip, stateful workflow rail, and disabled `Apply Nudge` state until the confidence-gated suggestion is actionable.
- Added confidence gating so weak or silent Guide/Dub signals no longer show bogus numeric offset/suggested nudge certainty.
- Added an `Apply Nudge` workflow action that only applies a suggestion when the confidence gate is reliable.
- Exposed the existing `Guide Blend` and `Stereo Focus` parameters in the Control Room UI.
- Expanded the editor size/layout to fit the extra controls and prevent workflow rail label clipping.
- Rebuilt the V1 roadmap around transparent delay-safe nudge, trust-meter alignment, and vocal-stack consonant control.
- Updated README with a producer quick-start, preview-build safety note, and V1 differentiator list.
- Updated the landing page with concrete producer workflow copy and a compact V1 focus section.

## v0.3.0

- Polished the landing page with v0.3.0 release CTAs, a dedicated release section, and a clearer product preview.
- Refined the plugin timing panel with a v0.3 preview badge and manual-versus-suggested nudge context.
- Bumped CMake, packaging scripts, release scripts, docs, and landing copy to `0.3.0`.
- Prepared the macOS Standalone, VST3, AU, installer, and bundle archive release lane for GitHub.

## Earlier modernization work

- Reframed the product as `Buffle Audio Align`, a vocal-stack alignment and articulation cleanup plugin.
- Replaced the generated `Hello World` editor with a modern Guide/Dub monitoring cockpit.
- Added APVTS-backed parameters for Tightness, Naturalness, Consonant Level, Manual Nudge, Preview Level, Guide Blend, and Stereo Focus.
- Added state save/restore through the processor ValueTree.
- Added lightweight live Guide/Dub level history and match-confidence visualization.
- Added refreshed README, roadmap, build notes, and dependency/licensing guidance.
- Documented the current build blocker: generated Xcode files still point to a missing local JUCE modules path.
- Added a modern CMake build using the local JUCE checkout at `/Users/hostin/vibecoding/waveform-visualizer/JUCE`.
- Built Debug and Release Standalone, VST3, and AU targets successfully.
- Added `scripts/build_and_package_macos.sh` to stage bundles and create `dist/BuffleAudioAlign-0.3.0-macOS.pkg`.
- Verified staged `.app`, `.vst3`, and `.component` bundles with ad-hoc codesigning.
- Added optional `Guide` sidechain bus support while treating the main input as `Dub`.
- Made `Manual Nudge` audible with a preallocated realtime-safe delay buffer.
- Added the Buffle Audio logo asset from `waveform-visualizer` and matched the Visualize dark/teal brand palette.
- Added a clickable branded About panel with product, version, workflow, website, and contact details.
- Polished the plugin UI with responsive Guide/Dub readouts, clearer match status, peak lane meters, formatted slider values, tooltips, workflow stepper styling, and transient button feedback.
- Added a standalone `BuffleAlignDSP` library with envelope extraction, global timing-offset estimation, manual nudge delay, and CTest coverage for silence, impulse onset, shifted-click offset recovery, and delay timing.
- Surfaced initial suggested nudge, signed offset, offset confidence, and Guide sidechain/fallback state in the plugin UI.
- Refreshed the legacy `.jucer` name/module path and raised the minimum editor height to protect the control-room layout.
- Reworked the README into a product/developer guide and added a standalone static landing page under `landing/`.
- Added helper scripts for serving/exposing the landing page and publishing GitHub release artifacts.
