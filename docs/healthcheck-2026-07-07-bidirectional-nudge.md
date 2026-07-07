# Buffle Audio Align Healthcheck - 2026-07-07 Bidirectional Nudge

## Scope

This healthcheck covers the post-`v0.3.0` bidirectional nudge slice:

- `BidirectionalNudge` DSP helper for fixed-latency early/late nudge planning.
- `Manual Nudge` APVTS range changed to `-120..+120 ms`.
- Processor reports a fixed max-nudge host latency and varies internal delay around that latency.
- Signed suggested nudge for reliable Guide/Dub offsets.
- Editor Apply Nudge and phrase health updated for signed timing corrections.
- Alignment Report and docs updated from positive-delay-first language to early/late timing correction language.

## Verified

```bash
cmake --build build/cmake-debug --target BuffleAlignDSPTests --config Debug --parallel
ctest --test-dir build/cmake-debug --output-on-failure
cmake --build build/cmake-debug --config Debug --parallel
```

Result: Debug Standalone, VST3, AU, shared code, DSP library, and DSP tests build successfully. `BuffleAlignDSPTests` passes.

## Implemented In This Slice

- `BidirectionalNudge` maps signed nudge to `hostLatencySamples + signedNudgeSamples`.
- The plugin reports fixed latency equal to the maximum nudge window so negative nudge can advance the Dub relative to host compensation.
- Reliable positive offsets now suggest negative timing corrections, and reliable negative offsets suggest positive corrections.
- Apply Nudge handles either sign.
- UI copy uses `Nudge Dub`, `Delay`, `Advance`, and signed apply labels.
- Tests cover reverse offset estimation, signed nudge planning, and signed report wording.

## Still Open For V1

- This is preview-grade latency compensation and still needs pluginval, AU validation, and DAW host validation.
- Existing `nudge` parameter range changed from `0..120` to `-120..+120`; old preview automation lanes should be checked manually.
- Capture/analyze/preview buttons remain workflow hints, not a complete phrase capture state machine.
- The `.pkg` preview artifact still needs AppleDouble payload cleanup before signing/notarization.
