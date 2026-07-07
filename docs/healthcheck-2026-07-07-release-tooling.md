# Buffle Audio Align Healthcheck - 2026-07-07 Release Tooling

## Scope

This healthcheck covers the post-`v0.3.0` release tooling hardening slice:

- Added repository `VERSION` file as the shared release version source.
- CMake now reads the product version from `VERSION`.
- Build script reads `VERSION` and supports `CLEAN_DIST=1` for release-candidate hygiene.
- Publish script supports `RELEASE_MODE=zip`, `RELEASE_MODE=pkg`, and `RELEASE_MODE=full`.
- Publish script now performs clean-tree, staged-bundle, release-notes, package-hygiene, archive-hygiene, and existing-tag preflights.
- Publish script defaults to draft prerelease output until V1 gates pass.
- Added `docs/release-evidence-template.md` for auditable release-candidate records.
- README, build notes, release inventory, roadmap, changelog, and release notes updated.

## Verification Commands

```bash
bash -n scripts/build_and_package_macos.sh scripts/publish_github_release.sh scripts/expose_landing_cloudflared.sh scripts/serve_landing.sh
cmake -S . -B build/cmake-debug -DJUCE_PATH=/Users/hostin/vibecoding/waveform-visualizer/JUCE -DCMAKE_BUILD_TYPE=Debug -DBUFFLE_BUILD_TESTS=ON
cmake --build build/cmake-debug --target BuffleAlignDSPTests --config Debug --parallel
ctest --test-dir build/cmake-debug --output-on-failure
cmake --build build/cmake-debug --config Debug --parallel
git diff --check
```

## Still Open For V1

- Developer ID Application and Installer identities are still required for signed release artifacts.
- Notarization, stapling, clean-account install smoke, AU validation, pluginval, and DAW host latency validation still need current evidence.
- Future release candidates should copy `docs/release-evidence-template.md` and fill it with command output and host validation results.
