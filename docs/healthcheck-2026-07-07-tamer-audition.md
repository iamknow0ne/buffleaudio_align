# Healthcheck - Tamer Audition

Date: 2026-07-07

## Scope

This slice adds a consonant-only removed-material audition path so producers can hear what Consonant Tamer reduced separately from the full timing/tamer `All Diff` preview.

## Verified Behavior

- `Preview Mode` now includes `Original`, `Aligned`, `Difference`, and `Tamer`.
- `Tamer` renders the post-timing, pre-Consonant-Tamer Dub minus the post-tamer Dub, isolating only Consonant Tamer reductions.
- The editor now shows `All Diff` and `Tamer` preview buttons in the existing left rail without adding layout panels.
- The changed-material strip shows both broad all-change amount and tamer-only amount.
- `Copy Report` includes `Consonant removed` and `Peak consonant removed` fields.
- DSP tests cover `PreviewMode::consonantRemoved`, real Consonant Tamer output feeding the removed-material meter, and report formatting for the new fields.

## Local Verification

```bash
cmake --build build/cmake-debug --target BuffleAlignDSPTests --config Debug --parallel
ctest --test-dir build/cmake-debug --output-on-failure
cmake --build build/cmake-debug --config Debug --parallel
```

## Remaining V1 Risk

- This is a first consonant-only audition, not a full visual consonant collision timeline.
- DAW host validation, AU validation, pluginval, signing, notarization, and clean-account install smoke remain V1 release gates.
