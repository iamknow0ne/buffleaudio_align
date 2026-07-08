# Report Export And Breath Mask Healthcheck - 2026-07-08

This records the V1 polish slice that added saveable alignment reports, compact decision-copy UI, and Breath Preservation Mask v0.

## Scope

- Added `Save` beside `Copy` in the workflow rail so the current alignment report can be written as a `.txt` file.
- Reused the existing canonical `AlignmentReport` formatter for both clipboard and file export.
- Added `BreathPreservationMask` as a pure DSP helper used inside `ConsonantTamer`.
- Protected soft sustained breath-like Dub material while keeping sharp unmatched bursts eligible for tamer reduction.
- Shortened central UI copy for `Next Best Move`, `Naturalness`, and `Articulation` strips to reduce clipping at compact editor sizes.
- Updated README, roadmap, release notes, changelog, tester guide, and landing copy around product-facing behavior.

## Verification

```bash
cmake --build build/cmake-debug --target BuffleAlignDSPTests --config Debug --parallel
ctest --test-dir build/cmake-debug --output-on-failure
```

Result: `100% tests passed, 0 tests failed out of 1`.

## Product Boundary

- Breath Preservation Mask v0 is a heuristic protection layer for soft sustained breath-like material. It is not phoneme-aware, lyric-aware, or a full breath detector.
- Save Report is a text export of the same current session read used by Copy Report; it is not a multi-phrase session database.
- External V1 gates still remain: Developer ID signing, notarization, clean-account install smoke, DAW timing matrix, and fresh packaged release artifacts.
