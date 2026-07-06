# Changelog

## v0.3.0

- Polished the landing page with v0.3.0 release CTAs, a dedicated release section, and a clearer product preview.
- Refined the plugin timing panel with a v0.3 preview badge and manual-versus-suggested nudge context.
- Bumped CMake, packaging scripts, release scripts, docs, and landing copy to `0.3.0`.
- Prepared the macOS Standalone, VST3, AU, installer, and bundle archive release lane for GitHub.

## Unreleased

- Added Buy Me a Coffee support CTAs to the landing page.
- Refreshed live landing and GitHub release screenshots for v0.3.0.
- Expanded README, release inventory, deployment notes, and healthcheck documentation.

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
