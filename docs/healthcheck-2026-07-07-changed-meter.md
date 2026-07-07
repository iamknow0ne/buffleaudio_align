# Buffle Audio Align Healthcheck - 2026-07-07 Changed Meter

## Scope

This healthcheck covers the post-`v0.3.0` changed-material metering slice:

- `RemovedMaterialMeter` DSP helper for processed-vs-original preview deltas.
- Processor snapshot exposure for changed amount and peak delta.
- Compact `CHANGED` meter in the editor.
- Copy Report changed-material stats.
- README, roadmap, release notes, changelog, and landing synchronization.

## Verified

```bash
cmake --build build/cmake-debug --target BuffleAlignDSPTests --config Debug --parallel
ctest --test-dir build/cmake-debug --output-on-failure
cmake --build build/cmake-debug --config Debug --parallel
```

Result: Debug Standalone, VST3, AU, shared code, DSP library, and DSP tests build successfully. `BuffleAlignDSPTests` passes.

## Implemented In This Slice

- `RemovedMaterialMeter` measures broad processed-vs-original preview delta amount, RMS delta, and peak delta.
- The meter runs after nudge and Consonant Tamer processing, before Preview Mode rendering mutates the output.
- The editor shows a compact `CHANGED` strip between timing readouts and Phrase Health.
- Copy Report includes changed-material and peak changed-material percentages.
- Tests cover identity, silence, gain-reduced transient, and channel-aware peak delta.

## Still Open For V1

- The current meter is a broad preview-delta meter, not a per-feature consonant-only removed-material solo.
- Suggested nudge is still positive-delay-first; bidirectional early/late compensation remains open.
- Capture/analyze/preview buttons remain workflow hints, not a complete phrase capture state machine.
- The `.pkg` preview artifact still needs AppleDouble payload cleanup before signing/notarization.
