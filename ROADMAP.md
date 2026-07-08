# Buffle Audio Align V1 Roadmap

V1 should not chase a full VocAlign/Revoice/Melodyne clone. The sharper wedge is a producer-controlled vocal-stack cleanup tool that makes timing decisions visible, preserves feel, and reduces doubled consonant clutter.

## Current State

Status: `v0.3.0` macOS preview, with additional V1 polish on `main`.

Done:

- Buffle Audio branded JUCE editor with About panel.
- Persistent APVTS parameters and state save/restore.
- Optional Guide sidechain bus plus Dub main input.
- Live Guide/Dub level history, signed offset estimate, confidence, and suggested nudge.
- Confidence-gated display so weak signals no longer show fake offset certainty.
- Bidirectional manual nudge in a standalone DSP module using fixed host latency compensation.
- Experimental Consonant Tamer Lite DSP for unmatched Dub consonant bursts, with Guide-matched attack preservation and Breath Preservation Mask v0 for soft sustained breath-like material.
- Stack Spread Governor v0 uses the `Stereo Focus` control as a lightweight mid/side width stage for role-aware doubles, stacks, choir layers, and ADR.
- Original / Aligned / All Diff / Tamer preview modes with test coverage for preview rendering.
- Changed-material meters for the overall processed-vs-original preview change and tamer-only consonant reduction, with CTest coverage for silence, identity, transient reduction, channel-aware peak delta, and real Consonant Tamer output.
- User-facing Stack Role presets for Double Tight, Choir Natural, Rap Stack, and ADR Loose.
- `Copy` and `Save` Report handoff summary for Phrase Health, Phrase advice, Trust Meter reason/advice, confidence, offset, suggested timing correction, changed-material amount, preview mode, stack role, and current controls.
- Phrase Health classifier, Trust Meter reason codes, stateful workflow rail, and `Next Best Move` card in the editor.
- Naturalness Risk Guardrail v0 for `Natural`, `Check Diff`, and `Too Much` states in UI and Copy Report.
- V1 tester guide with zip-first install guidance, DAW smoke flow, feedback checklist, and known preview limits.
- Host latency validation matrix for AU/VST3 DAW proof.
- CMake build for Standalone, VST3, AU, and DSP tests.
- macOS package and bundle archive generation.
- Release tooling reads `VERSION`, supports zip/pkg/full publish modes, and defaults preview publishes to draft prereleases.
- Cloudflare Pages landing page.

Not V1-ready yet:

- Bidirectional nudge is implemented as fixed-latency preview compensation, but still needs DAW host validation and clearer clean-session latency testing.
- A first Articulation Risk strip is implemented for suspected consonant clutter, but it is still a lightweight visual aid over transient/tamer evidence, not a full phoneme-aware detector or detailed collision timeline.
- Capture/analyze/preview buttons are now framed as a session checklist, but they are not a complete captured phrase state machine.
- Difference mode now has broad changed-material metering and Tamer mode solos Consonant Tamer reductions; richer per-feature timelines remain open.
- Stack role presets apply role-owned controls, and `Stereo Focus` now drives Stack Spread Governor v0. `Guide Blend` remains a monitoring/preview control until deeper guide-aware DSP lands.
- Alignment Report now supports clipboard copy and text-file save; richer per-feature removed-material metering remains open.
- Installer is not Developer ID signed or notarized.
- Host validation matrix exists, and local AU plus strict VST3 `pluginval` validation have current evidence, but DAW timing results, audio demo corpus, and golden WAV regression lane remain open.

## V1 Differentiators

These are the target features that make Align meaningfully different from generic alignment tools:

1. **Trust Meter Alignment**: show source state, Guide/Dub levels, offset direction, confidence, and why the recommendation is safe or unavailable.
2. **One-Click Safe Nudge**: apply suggested early/late timing nudge only when confidence and signal floors are credible. **Initial bidirectional latency-compensated path implemented.**
3. **Consonant Collision Detector**: highlight likely doubled consonant flams against the guide. **Initial Articulation Risk strip implemented; full phoneme-aware detection remains future work.**
4. **Consonant Tamer Lite**: fade-safe attenuation of consonant clutter without flattening vowels.
5. **Removed Material Audition**: solo the timing/consonant material being reduced through Difference preview, then add richer metering. **Broad All Diff and consonant-only Tamer audition implemented.**
6. **Naturalness Guardrail**: warn when a move risks over-tight, phasey, or sterile stacked vocals. **Initial policy layer implemented.**
7. **Guide Fallback Intelligence**: explain missing, weak, or ambiguous Guide states with stable Trust Meter reason codes instead of producing misleading numbers. **Initial route/listen/locked/safe-nudge states implemented.**
8. **Phrase Health Report**: classify each phrase as route/listen, safe nudge, print-ready, changed material, watch naturalness, watch articulation, or too much cleanup. **Initial classifier and report advice are implemented.**
9. **Stack Spread Governor**: preserve a musical millisecond spread for harmonies and gang vocals. **Initial `Stereo Focus` mid/side governor is implemented.**
10. **Breath Preservation Mask**: protect breaths, plosives, and expressive attacks from cleanup. **Initial soft sustained material protection is implemented inside Consonant Tamer.**
11. **Vowel-Only Micro-Warp Preview**: future constrained warping that stretches vowels while locking consonants.
12. **Harmony-Aware Tightness Presets**: `Double Tight`, `Choir Natural`, `Rap Stack`, `ADR Loose`. **Initial user-facing selector implemented.**
13. **Exportable Alignment Report**: phrase offset, confidence, nudge, and correction amount for session handoff. **Clipboard copy and text-file save are implemented.**

## Milestone A - Reliable Nudge Product

Goal: make the current analysis/nudge workflow trustworthy enough for daily testing.

- Keep all offset and suggested-nudge UI gated by confidence.
- Add one-click bidirectional safe nudge to the editor. **Initial signed Apply Nudge implemented.**
- Add clear states: route, quiet Guide, quiet Dub, listen, confidence locked, no usable guide, no timing nudge needed. **Initial Trust Meter reason codes implemented.**
- Make the editor's next action visible without reading docs. **Initial `Next Best Move` card implemented.**
- Add tests for confidence gating, silence, weak signal, and offset sign.
- Document DAW sidechain setup and unsigned preview safety.
- Verify Standalone build, DSP tests, and current package generation.

Exit criteria:

- Weak/silent input never displays a bogus numeric recommendation.
- A reliable signed offset can set the appropriate early/late `Nudge Dub` correction through the UI.
- README and landing explain the exact producer loop.

## Milestone B - Consonant Tamer Lite

Goal: deliver the first vocal-stack-specific differentiator.

- Add transient/high-frequency consonant feature extraction.
- Add fade-safe Dub attenuation controlled by `Consonant Tamer`. **Initial realtime slice implemented.**
- Add first consonant-collision visual aid from the current transient/tamer evidence. **Initial Articulation Risk strip implemented.**
- Add removed-material audition and meter foundations. **Initial Difference preview and changed-material meter implemented.**
- Add consonant-only removed-material audition. **Initial Tamer preview implemented.**
- Add naturalness risk guardrail for over-cleaning. **Initial UI/report policy layer implemented.**
- Add breath/attack preservation heuristics. **Initial Breath Preservation Mask v0 implemented.**
- Add synthetic tests for consonant bursts, breath-like noise, and fade boundaries. **Core burst/silence/sustain/Guide-match/breath-bed tests implemented.**

Exit criteria:

- Consonant cleanup is audible, controllable, and does not smear sustained vowels.
- Tests prove silence and non-consonant sustained material remain stable.

## Milestone C - Workflow Productization

Goal: make Align feel like a practical DAW tool rather than a meter.

- Make the workflow rail stateful: Route, Listen, Preview, Tame, Print.
- Add phrase health cards and error states.
- Add tester-facing session guidance in the app. **Initial session-flow rail details and `Next Best Move` card implemented.**
- Add A/B controls: Original, Aligned, Difference/Removed. **Initial Original / Aligned / All Diff / Tamer controls implemented.**
- Add stack role presets. **Initial Stack Role selector implemented.**
- Add stack spread governor. **Initial `Stereo Focus` mid/side governor implemented.**
- Add session handoff report. **Copy Report and Save Report text export implemented.**
- Add compatibility matrix for Standalone, VST3, AU, macOS, Apple Silicon/Intel, and tested DAWs.
- Add tester guide and feedback checklist. **Initial V1 tester guide implemented.**

Exit criteria:

- A new producer can route, analyze, preview, and prepare a host-side print without reading build docs.
- Docs include a tester feedback checklist.

## Milestone D - V1 Release Candidate

Goal: ship a clean, supportable macOS V1.

- Pin or vendor JUCE, or provide a reproducible bootstrap.
- Verify package payload hygiene for each installer candidate so `.pkg` contains no `.DS_Store` or AppleDouble `._*` files. **Build script repair-and-verify gate implemented.**
- Add Developer ID Application and Installer signing.
- Notarize and staple installer.
- Run install smoke on a clean macOS account.
- Run AU validation and at least one VST3 host load test. **Local AU `auval -v aufx BfAl BfAu` passed on July 7, 2026; VST3 `pluginval --strictness-level 10` passed on July 8, 2026. DAW host matrix remains open.**
- Execute the host latency validation matrix in [docs/validation-host-latency.md](docs/validation-host-latency.md).
- Publish GitHub release with SHA256 checksums.
- Attach a completed release evidence record using [docs/release-evidence-template.md](docs/release-evidence-template.md).
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
