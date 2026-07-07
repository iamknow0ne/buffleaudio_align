# Healthcheck - Trust Diagnostics

Date: 2026-07-07

## Scope

Trust Diagnostics v1 centralizes the phrase-health decision policy behind the editor, processor workflow status, and clipboard report.

## Verified Behavior

- Trust Meter reason codes cover route, quiet Guide, quiet Dub, listening, locked, safe delay, and safe advance states.
- The editor uses the shared trust state for phrase health, workflow rail details, next-best-move advice, and unavailable Apply Nudge feedback.
- `Copy Report` includes `Phrase health`, `Trust reason`, and `Trust advice` before confidence, offset, suggested nudge, preview, role, and control values.
- Unreliable timing reports hide stale offset/nudge numbers even when internal values are nonzero.

## Local Verification

```bash
cmake --build build/cmake-debug --target BuffleAlignDSPTests --config Debug --parallel
ctest --test-dir build/cmake-debug --output-on-failure
cmake --build build/cmake-debug --config Debug --parallel
git diff --check
```

## Remaining V1 Risk

- Trust Diagnostics are deterministic and unit-tested, but DAW-side validation is still needed for AU/VST3 sidechain routing, latency compensation, and clean-session recall.
- Installer signing, notarization, pluginval, AU validation, and clean-account install smoke remain V1 release gates.
