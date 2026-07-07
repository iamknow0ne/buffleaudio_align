# Buffle Audio Align Release Notes

## Post-v0.3.0 Source Polish

These changes are in the current source tree after the public `v0.3.0` GitHub release. They should be included in the next preview release after fresh artifacts are built and verified.

- Latency-compensated bidirectional `Nudge Dub` support for early/late timing corrections.
- Trust Diagnostics v1: shared Trust Meter reason codes and action advice for route, quiet-lane, listening, locked, delay, and advance states.
- Session-flow rail and `Next Best Move` card for tester-friendly phrase workflow guidance.
- Naturalness Risk Guardrail v0 for `Natural`, `Check Diff`, and `Too Much` UI/report states.
- Signed Apply Nudge states and report text for reliable early/late Guide/Dub offsets.
- Additional deterministic DSP regression coverage for nudge edge clamps, Consonant Tamer block-size invariance, changed-material calibration, and guardrail policy.
- Release tooling now reads `VERSION` and supports zip/pkg/full publish modes with draft prerelease defaults.
- Clipboard `Copy Report` handoff summaries for phrase health, confidence, offset, suggested timing correction, changed-material amount, preview mode, stack role, and current controls.
- Report formatter tests for unavailable and safe-nudge states.
- Initial changed-material meter for overall processed-vs-original preview change, with DSP tests and editor/report surfacing.
- Clearer first-run UI labels: Arm Listen, Check Timing, Preview Align, dynamic Apply Nudge text, and quiet-lane hints.
- Stack Role presets for Double Tight, Choir Natural, Rap Stack, and ADR Loose.
- Role presets apply Tightness, Naturalness, Consonant Tamer, Guide Blend, and Stereo Focus without changing Nudge or Preview Mode. Naturalness and Consonant Tamer are the most audible pieces today; deeper Guide Blend and Stereo Focus DSP remains V1 work.
- Original / Aligned / Difference preview modes with persistent APVTS state.
- Difference audition path for hearing the processed-minus-original material. Initial changed-material metering is in place; per-feature removed-material solo remains V1 work.
- Updated standalone screenshot showing the A/B preview controls.
- Experimental Consonant Tamer Lite DSP for unmatched Dub consonant bursts.
- Phrase-health strip, stateful workflow rail, and disabled Apply Nudge state until a safe suggestion exists.
- CTest coverage for the consonant-tamer preview slice.
- Confidence-gated offset and suggested nudge display.
- `Apply Nudge` workflow action for reliable suggestions.
- Visible `Guide Blend` and `Stereo Focus` controls.
- V1 roadmap reframed around trust-meter alignment, delay-safe nudge, and consonant cleanup.
- Landing, README, host-latency validation, and V1 tester guide updated with the producer workflow and current V1 focus.

## v0.3.0

Developer preview for macOS Standalone, VST3, and AU.

## Highlights

- Polished Buffle Audio Align editor with Guide/Dub monitoring and a clearer v0.3 preview state.
- Optional Guide sidechain with Dub fallback indication.
- Manual nudge delay preview with manual-versus-suggested nudge context.
- Initial signed offset estimate, offset confidence, and suggested nudge readouts.
- Standalone DSP library with CTest coverage for envelope extraction, offset estimation, and nudge timing.
- macOS Standalone, VST3, AU, local `.pkg` installer, and zipped staged bundles.
- Polished static landing page under `landing/`, designed to be exposed without serving the full repository.

## Artifacts

- `BuffleAudioAlign-0.3.0-macOS.pkg`
- `BuffleAudioAlign-0.3.0-macOS-bundles.zip`

## Notes

The bundle zip is the preferred preview testing path. The bundles are ad-hoc signed for local verification. The installer is not Developer ID signed or notarized yet.
