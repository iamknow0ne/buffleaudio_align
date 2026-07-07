# Buffle Audio Align Healthcheck - 2026-07-07 Naturalness And Validation

## Scope

This healthcheck covers the post-`v0.3.0` naturalness and validation credibility slice:

- `NaturalnessGuardrail` DSP policy helper for `Natural`, `Check Diff`, and `Too Much` states.
- Editor Naturalness strip and `Next Best Move` integration.
- Copy Report includes naturalness risk and advice.
- Deterministic DSP regression tests for:
  - bidirectional nudge edge clamps,
  - Consonant Tamer block-size invariance,
  - changed-material calibration,
  - Naturalness Guardrail policy states.
- `docs/validation-host-latency.md` added as the AU/VST3 host-latency validation matrix for V1.
- README, roadmap, release docs, tester guide, and landing page updated.

## Verification Commands

```bash
cmake --build build/cmake-debug --target BuffleAlignDSPTests --config Debug --parallel
ctest --test-dir build/cmake-debug --output-on-failure
cmake --build build/cmake-debug --config Debug --parallel
bash -n scripts/build_and_package_macos.sh scripts/publish_github_release.sh scripts/expose_landing_cloudflared.sh scripts/serve_landing.sh
git diff --check
```

## Still Open For V1

- Naturalness Guardrail is a first policy layer; it does not replace per-feature removed-material solo or a full consonant collision timeline.
- Host latency validation still needs to be executed in real AU/VST3 DAWs.
- Release evidence, signing, notarization, clean-account install smoke, AU validation, and pluginval remain V1 blockers.
