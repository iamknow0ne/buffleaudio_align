# Stack Spread Governor Healthcheck - 2026-07-08

This records the V1 slice that made `Stereo Focus` audible through a lightweight mid/side width governor.

## Scope

- Added `BufflePlug-Analyzer/Source/DSP/StackSpreadGovernor.*`.
- Integrated Stack Spread Governor after timing nudge and before Consonant Tamer.
- Kept `Tamer` audition scoped to consonant removal by applying the width stage before the pre-tamer reference is captured.
- Added DSP tests for mono no-op behavior, high-focus narrowing, and loose-role width preservation.
- Updated README, roadmap, release notes, changelog, tester guide, and landing copy.

## Verification

```bash
cmake --build build/cmake-debug --target BuffleAlignDSPTests --config Debug --parallel
ctest --test-dir build/cmake-debug --output-on-failure
```

Result: `100% tests passed, 0 tests failed out of 1`.

## Product Boundary

- Stack Spread Governor v0 is a simple mid/side width stage driven by `Stereo Focus`.
- It is not a full harmony-aware spatial engine, phrase-aware spread model, or DAW-specific width automation system.
- Real DAW validation is still required before broad V1 release claims.
