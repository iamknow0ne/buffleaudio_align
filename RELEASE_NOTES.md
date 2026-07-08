# Buffle Audio Align Release Notes

## v0.4.0 Preview

Post-`v0.3.0` macOS preview for Standalone, VST3, and AU. This is still a preview release: the installer is not Developer ID signed or notarized, and final V1 still needs clean-account install smoke plus DAW latency matrix evidence.

- Product-first landing page refresh centered on the actual Align workflow and current standalone UI.
- Breath Preservation Mask v0 inside Consonant Tamer to better protect soft sustained breath-like material while reducing sharper unmatched Dub attacks.
- Stack Spread Governor v0: `Stereo Focus` now drives a lightweight mid/side width stage for tighter doubles or wider choir/ADR-style layers.
- Phrase Health classifier separates the producer verdict from the Trust Meter reason, adding Phrase advice to reports and the central status strip.
- Saveable alignment reports for session notes and tester feedback, alongside clipboard copy.
- Compact central UI guidance for Naturalness, Articulation, and Next Best Move so the decision surface is easier to scan at smaller editor sizes.
- Latency-compensated bidirectional `Nudge Dub` support for early/late timing corrections.
- Trust Diagnostics v1: shared Trust Meter reason codes and action advice for route, quiet-lane, listening, locked, delay, and advance states.
- Consonant-only `Tamer` audition mode plus report fields for tamer-only removed material, separate from broad `All Diff` preview changes.
- Session-flow rail and `Next Best Move` card for tester-friendly phrase workflow guidance.
- Naturalness Risk Guardrail v0 for `Natural`, `Check Diff`, and `Too Much` UI/report states.
- Signed Apply Nudge states and report text for reliable early/late Guide/Dub offsets.
- Additional deterministic DSP regression coverage for nudge edge clamps, Consonant Tamer block-size invariance, changed-material calibration, and guardrail policy.
- Release tooling now reads `VERSION` and supports zip/pkg/full publish modes with draft prerelease defaults.
- Clipboard and text-file report handoff summaries for Phrase Health, Phrase advice, confidence, offset, suggested timing correction, changed-material amount, preview mode, stack role, and current controls.
- Report formatter tests for unavailable and safe-nudge states.
- Initial changed-material meter for overall processed-vs-original preview change, with DSP tests and editor/report surfacing.
- Clearer first-run UI labels: Arm Listen, Check Timing, Preview Align, dynamic Apply Nudge text, and quiet-lane hints.
- Stack Role presets for Double Tight, Choir Natural, Rap Stack, and ADR Loose.
- Role presets apply Tightness, Naturalness, Consonant Tamer, Guide Blend, and Stereo Focus without changing Nudge or Preview Mode. Stereo Focus now drives the initial Stack Spread Governor; Guide Blend now adds Guide sidechain monitor blend for timing checks.
- Original / Aligned / Difference preview modes with persistent APVTS state.
- Difference audition path for hearing the processed-minus-original material. Initial changed-material metering is in place; per-feature removed-material solo remains V1 work.
- Updated standalone screenshots showing the current product surface and A/B preview controls.
- Experimental Consonant Tamer Lite DSP for unmatched Dub consonant bursts.
- Phrase-health strip, stateful workflow rail, and disabled Apply Nudge state until a safe suggestion exists.
- CTest coverage for the consonant-tamer preview slice.
- Confidence-gated offset and suggested nudge display.
- `Apply Nudge` workflow action for reliable suggestions.
- Visible `Guide Blend` and `Stereo Focus` controls.
- Roadmap reframed around trust-meter alignment, delay-safe nudge, and consonant cleanup.
- Landing, README, host-latency validation, and tester guide updated with the producer workflow and current preview focus.

## Artifacts

- `BuffleAudioAlign-0.4.0-macOS.pkg`
- `BuffleAudioAlign-0.4.0-macOS-bundles.zip`
- `BuffleAudioAlign-0.4.0-SHA256SUMS.txt`

## v0.3.0

MacOS preview for Standalone, VST3, and AU.

## Highlights

- Polished Buffle Audio Align editor with Guide/Dub monitoring and a clearer v0.3 preview state.
- Optional Guide sidechain with Dub fallback indication.
- Manual nudge delay preview with manual-versus-suggested nudge context.
- Initial signed offset estimate, offset confidence, and suggested nudge readouts.
- Standalone DSP library with CTest coverage for envelope extraction, offset estimation, and nudge timing.
- macOS Standalone, VST3, AU, local `.pkg` installer, and zipped staged bundles.
- Polished static landing page under `landing/`, designed to be exposed without serving the full repository.

## v0.3.0 Artifacts

- `BuffleAudioAlign-0.3.0-macOS.pkg`
- `BuffleAudioAlign-0.3.0-macOS-bundles.zip`

## Notes

The bundle zip is the preferred preview testing path. The bundles are ad-hoc signed for local verification. The installer is not Developer ID signed or notarized yet.
