# Processor Latency Contract Healthcheck - 2026-07-08

This records the local hostless test coverage added for Align's plugin-side latency contract.

## Scope

- Added `tests/ProcessorLatencyContractTests.cpp`.
- Wired `BuffleAlignProcessorLatencyContractTests` into CTest.
- Added the processor contract test target to `scripts/v1_local_preflight.sh`.

## Covered Locally

- `prepareToPlay()` reports the documented `120 ms` host latency at common sample rates:
  - `44100 Hz -> 5292 samples`
  - `48000 Hz -> 5760 samples`
  - `96000 Hz -> 11520 samples`
- `Manual Nudge` maps to `-120 ms`, `0 ms`, and `+120 ms`.
- State save/restore preserves a negative nudge value.
- `scripts/v1_local_preflight.sh` now builds both `BuffleAlignDSPTests` and `BuffleAlignProcessorLatencyContractTests`.

## Verification

```bash
ctest --test-dir build/cmake-debug --output-on-failure
scripts/v1_local_preflight.sh
```

Both commands passed locally on July 8, 2026.

## Still External

This does not replace the DAW latency matrix. It proves the plugin-side contract before testing host plugin delay compensation behavior in Logic, Reaper, Ableton, or other DAWs.
