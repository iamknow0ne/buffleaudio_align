# Codebase Healthcheck - 2026-07-06

Scope: Buffle Audio Align v0.3.0 source, landing page, local release artifacts, and public GitHub release.

## Status

- Git branch clean before this documentation/screenshot pass: `main...origin/main`.
- Shell scripts parse successfully with `bash -n`.
- No stale `0.2.0`, `v0.2.0`, `BuffleAudioAlign-0.2.0`, or legacy `JUCE_APP_VERSION=1.0.0` markers in tracked source paths outside ignored build/dist output.
- Debug CMake build passes for Standalone, VST3, and AU.
- DSP CTest passes in Debug and Release.
- Staged Standalone, VST3, and AU bundles pass `codesign --verify --deep --strict`.
- Staged bundle metadata reports `CFBundleShortVersionString` and `CFBundleVersion` as `0.3.0`.
- `dist/BuffleAudioAlign-0.3.0-macOS-bundles.zip` contains no `.DS_Store` or AppleDouble `._*` entries.
- GitHub release `v0.3.0` exists and includes the `.pkg` installer plus bundles archive.
- Public landing page serves the v0.3.0 copy and release links.
- Cloudflare Pages configuration is present in `wrangler.toml` and points to `landing/`.
- Cloudflare Tunnel is serving only the local `landing/` directory through the local static server.

## Commands Run

```bash
bash -n scripts/build_and_package_macos.sh scripts/expose_landing_cloudflared.sh scripts/publish_github_release.sh scripts/serve_landing.sh
rg -n "0\\.2\\.0|v0\\.2\\.0|BuffleAudioAlign-0\\.2\\.0|JUCE_APP_VERSION=1\\.0\\.0|TODO|FIXME" . --glob '!build/**' --glob '!dist/**' --glob '!.git/**'
cmake --build build/cmake-debug --config Debug --parallel
ctest --test-dir build/cmake-debug --output-on-failure
ctest --test-dir build/cmake-release --output-on-failure
codesign --verify --deep --strict --verbose=2 "dist/stage/Buffle Audio Align.app"
codesign --verify --deep --strict --verbose=2 "dist/stage/Buffle Audio Align.vst3"
codesign --verify --deep --strict --verbose=2 "dist/stage/Buffle Audio Align.component"
pkgutil --check-signature dist/BuffleAudioAlign-0.3.0-macOS.pkg
unzip -l dist/BuffleAudioAlign-0.3.0-macOS-bundles.zip | rg '/\\._|\\.DS_Store'
gh release view v0.3.0 --repo iamknow0ne/buffleaudio_align --json url,tagName,name,assets
npx wrangler@latest pages deploy landing --project-name=buffleaudio-align --branch=main
```

## Known Risks

- The installer is not Developer ID Installer signed or notarized yet. `pkgutil --check-signature` reports `Status: no signature`, which is expected for the current preview lane.
- Local ignored `dist/` still contains older v0.2.0 artifacts alongside v0.3.0 artifacts. They are not tracked, but cleaning the local distribution folder before each release would reduce operator confusion.
- JUCE is still resolved from `/Users/hostin/vibecoding/waveform-visualizer/JUCE`; pinning or vendoring JUCE remains the biggest reproducibility improvement.
- Legacy Projucer/Xcode naming still exists in generated project structure. CMake remains canonical.
- Cloudflare Pages deployment requires `CLOUDFLARE_API_TOKEN` in this non-interactive session. The Pages deploy attempt stopped before upload because no token was present.

## Fresh Screenshots

- Landing page: `landing/assets/screenshots/landing-v0.3.0.jpg`
- GitHub release page: `landing/assets/screenshots/github-release-v0.3.0.jpg`
