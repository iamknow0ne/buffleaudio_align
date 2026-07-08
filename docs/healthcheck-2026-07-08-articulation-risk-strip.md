# Articulation Risk Strip Healthcheck - 2026-07-08

This records the first lightweight visual aid for suspected consonant clutter in Buffle Audio Align.

## Scope

- Added pure DSP classifier: `BufflePlug-Analyzer/Source/DSP/ArticulationRisk.*`
- Added snapshot/report fields for `ArticulationRisk` and score.
- Added an `ARTICULATION` strip in the main timing view.
- Added Copy Report output for articulation risk.
- Added DSP tests for listening, clean, watch, and collision states.

## Product Boundary

The strip is intentionally conservative. It uses existing reliable-offset confidence and tamer/consonant-removal evidence to indicate likely articulation clutter. It is not a full phoneme detector, exact consonant timeline, ML classifier, or automatic repair system.

## Acceptance

- Missing or unreliable Guide/Dub timing reports `Listening` instead of fake certainty.
- Gentle timing/tamer movement reports `Clean`.
- Medium consonant/timing pressure reports `Watch consonants`.
- Larger timing pressure plus high consonant removal reports `Collision risk`.
- Tester workflow remains: confirm the visual strip with `Tamer` audition and `All Diff`.

## Verification

Run:

```bash
cmake --build build/cmake-debug --target BuffleAlignDSPTests --config Debug --parallel
ctest --test-dir build/cmake-debug --output-on-failure
```

## Still Open For V1

- A true phoneme-aware consonant collision detector.
- A detailed visual collision timeline aligned to phrase position.
- Host DAW validation for how the strip behaves during real sidechain workflows.
