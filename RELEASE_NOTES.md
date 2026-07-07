# Buffle Audio Align 0.3.0

Developer preview for macOS Standalone, VST3, and AU.

## Current Unreleased Polish

- Experimental Consonant Tamer Lite DSP for unmatched Dub consonant bursts.
- Phrase-health strip, stateful workflow rail, and disabled Apply Nudge state until a safe suggestion exists.
- CTest coverage for the consonant-tamer preview slice.
- Confidence-gated offset and suggested nudge display.
- `Apply Nudge` workflow action for reliable suggestions.
- Visible `Guide Blend` and `Stereo Focus` controls.
- V1 roadmap reframed around trust-meter alignment, safe nudge, and consonant cleanup.
- Landing and README updated with the producer workflow and Road to V1 story.

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

The bundles are ad-hoc signed for local verification. The installer is not Developer ID signed or notarized yet.
