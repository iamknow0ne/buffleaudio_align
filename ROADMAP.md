# Buffle Audio Align V1 Roadmap

V1 should not chase a full VocAlign/Revoice/Melodyne clone. The sharper wedge is a producer-controlled vocal-stack cleanup tool that makes timing decisions visible, preserves feel, and reduces doubled consonant clutter.

## Current State

Status: `v0.3.0` developer preview.

Done:

- Buffle Audio branded JUCE editor with About panel.
- Persistent APVTS parameters and state save/restore.
- Optional Guide sidechain bus plus Dub main input.
- Live Guide/Dub level history, signed offset estimate, confidence, and suggested nudge.
- Confidence-gated display so weak signals no longer show fake offset certainty.
- Manual nudge delay in a standalone DSP module.
- Experimental Consonant Tamer Lite DSP for unmatched Dub consonant bursts, with Guide-matched attack preservation.
- Original / Aligned / Difference preview modes with test coverage for preview rendering.
- User-facing Stack Role presets for Double Tight, Choir Natural, Rap Stack, and ADR Loose.
- Phrase-health strip and stateful workflow rail in the editor.
- CMake build for Standalone, VST3, AU, and DSP tests.
- macOS package and bundle archive generation.
- Cloudflare Pages landing page.

Not V1-ready yet:

- Suggested nudge is still positive-delay-first; early/late compensation needs a fuller dual-path design.
- Consonant Tamer is implemented as a first realtime transient tamer, but it is not yet a full consonant collision detector or removed-material audition workflow.
- Capture/analyze/preview buttons are workflow hints, not a complete phrase state machine.
- Difference mode is implemented as a first removed-material audition path, but it does not yet have dedicated metering or per-feature solo controls.
- Stack role presets apply role-owned controls, but guide blend and stereo focus are still mostly monitoring/shape controls until deeper stereo DSP lands.
- Installer is not Developer ID signed or notarized.
- No DAW host validation matrix, audio demo corpus, or golden WAV regression lane yet.

## V1 Differentiators

These are the target features that make Align meaningfully different from generic alignment tools:

1. **Trust Meter Alignment**: show source state, Guide/Dub levels, offset direction, confidence, and why the recommendation is safe or unavailable.
2. **One-Click Safe Nudge**: apply suggested nudge only when confidence and signal floors are credible.
3. **Consonant Collision Detector**: highlight doubled consonants that flam against the guide.
4. **Consonant Tamer Lite**: fade-safe attenuation of consonant clutter without flattening vowels.
5. **Removed Material Audition**: solo the timing/consonant material being reduced through Difference preview, then add richer metering.
6. **Naturalness Guardrail**: warn when a move risks over-tight, phasey, or sterile stacked vocals.
7. **Guide Fallback Intelligence**: explain missing/weak sidechain routing instead of producing misleading numbers.
8. **Phrase Health Report**: classify each phrase as usable, weak guide, quiet dub, ambiguous, clipped, or unsafe nudge.
9. **Stack Spread Governor**: preserve a musical millisecond spread for harmonies and gang vocals.
10. **Breath Preservation Mask**: protect breaths, plosives, and expressive attacks from cleanup.
11. **Vowel-Only Micro-Warp Preview**: future constrained warping that stretches vowels while locking consonants.
12. **Harmony-Aware Tightness Presets**: `Double Tight`, `Choir Natural`, `Rap Stack`, `ADR Loose`. **Initial user-facing selector implemented.**
13. **Exportable Alignment Report**: phrase offset, confidence, nudge, and correction amount for session handoff.

## Milestone A - Reliable Nudge Product

Goal: make the current analysis/nudge workflow trustworthy enough for daily testing.

- Keep all offset and suggested-nudge UI gated by confidence.
- Add one-click safe nudge to the editor.
- Add clear states: route, listen, confidence locked, no usable guide, no positive delay nudge needed.
- Add tests for confidence gating, silence, weak signal, and offset sign.
- Document DAW sidechain setup and unsigned preview safety.
- Verify Standalone build, DSP tests, and current package generation.

Exit criteria:

- Weak/silent input never displays a bogus numeric recommendation.
- A reliable negative offset can set `Manual Nudge` through the UI.
- README and landing explain the exact producer loop.

## Milestone B - Consonant Tamer Lite

Goal: deliver the first vocal-stack-specific differentiator.

- Add transient/high-frequency consonant feature extraction.
- Add fade-safe Dub attenuation controlled by `Consonant Tamer`. **Initial realtime slice implemented.**
- Add removed-material audition mode. **Initial Difference preview implemented.**
- Add breath/attack preservation heuristics.
- Add synthetic tests for consonant bursts, breath-like noise, and fade boundaries. **Core burst/silence/sustain/Guide-match tests implemented.**

Exit criteria:

- Consonant cleanup is audible, controllable, and does not smear sustained vowels.
- Tests prove silence and non-consonant sustained material remain stable.

## Milestone C - Workflow Productization

Goal: make Align feel like a practical DAW tool rather than a meter.

- Make the workflow rail stateful: Route, Listen, Preview, Tame, Print.
- Add phrase health cards and error states.
- Add A/B controls: Original, Aligned, Difference/Removed. **Initial Original / Aligned / Difference controls implemented.**
- Add stack role presets. **Initial Stack Role selector implemented.**
- Add compatibility matrix for Standalone, VST3, AU, macOS, Apple Silicon/Intel, and tested DAWs.

Exit criteria:

- A new producer can route, analyze, preview, and print without reading build docs.
- Docs include a tester feedback checklist.

## Milestone D - V1 Release Candidate

Goal: ship a clean, supportable macOS V1.

- Pin or vendor JUCE, or provide a reproducible bootstrap.
- Fix package payload hygiene so `.pkg` contains no `.DS_Store` or AppleDouble `._*` files.
- Add Developer ID Application and Installer signing.
- Notarize and staple installer.
- Run install smoke on a clean macOS account.
- Run AU validation and at least one VST3 host load test.
- Publish GitHub release with SHA256 checksums.
- Update landing, README, changelog, docs, screenshots, and release notes.

Exit criteria:

- GitHub V1 release artifacts are freshly built, signed/notarized where possible, checksummed, and documented.
- Landing and README match the shipped behavior.

## Post-V1

- Constrained DTW and monotonic warp maps.
- Vowel-only micro-warp preview renderer.
- ARA evaluation.
- AAX and CLAP evaluation.
- ML phoneme detection.
- MIDI/slice groove mode.
- Commercial time-stretch licensing decisions.
