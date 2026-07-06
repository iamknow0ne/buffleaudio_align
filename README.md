# Buffle Audio Align

Buffle Audio Align is a JUCE audio plugin for vocal-stack alignment and articulation cleanup. Put it on a Dub or backing-vocal track, feed or monitor a Guide vocal, estimate the timing relationship, then tighten the Dub while preserving the small performance details that make stacked vocals feel alive.

Current build target: macOS Standalone, VST3, and AU.

## What Works Now

- Branded Buffle Audio editor with the shared logo, dark teal visual identity, and in-plugin About panel.
- Persistent JUCE parameters through `AudioProcessorValueTreeState`.
- Session state save/restore.
- Optional `Guide` sidechain input bus, with the main input treated as `Dub`.
- Live Guide/Dub monitoring, signed offset estimate, offset confidence, and delay-safe suggested nudge.
- Realtime-safe manual nudge delay through a testable DSP module.
- Standalone DSP library with unit tests for envelope extraction, global offset estimation, and manual nudge timing.
- CMake build for Standalone, VST3, and AU.
- Local macOS `.pkg` installer generation.
- Static landing page in `landing/`, designed to be exposed without serving the whole repository.

## Product Shape

The v0.3.0 direction is intentionally narrow:

1. Monitor or capture Guide and Dub.
2. Extract energy/onset envelopes.
3. Estimate a global timing offset.
4. Preview or apply a manual/automatic nudge.
5. Add a lightweight consonant cleanup pass.
6. Defer full DTW, time-stretch rendering, ARA, ML phoneme detection, and MIDI groove mode until the capture/analyze/preview loop is trustworthy.

Tagline:

> Fast vocal stack cleanup: find the timing offset, nudge the double, tame the consonants.

## Build

The canonical build path is CMake. It currently uses the local JUCE checkout at:

```text
/Users/hostin/vibecoding/waveform-visualizer/JUCE
```

Override it with `JUCE_PATH=/path/to/JUCE`.

```bash
cmake -S . -B build/cmake-debug \
  -DJUCE_PATH=/Users/hostin/vibecoding/waveform-visualizer/JUCE \
  -DCMAKE_BUILD_TYPE=Debug \
  -DBUFFLE_BUILD_TESTS=ON

cmake --build build/cmake-debug --config Debug --parallel
ctest --test-dir build/cmake-debug --output-on-failure
```

## Package

```bash
scripts/build_and_package_macos.sh
```

Outputs:

- `dist/stage/Buffle Audio Align.app`
- `dist/stage/Buffle Audio Align.vst3`
- `dist/stage/Buffle Audio Align.component`
- `dist/BuffleAudioAlign-0.3.0-macOS.pkg`

The staged bundles are ad-hoc signed for local verification. The package is not Developer ID Installer signed or notarized yet.

## Landing Page

Production Pages URL:

```text
https://buffleaudio-align.pages.dev/
```

Support link:

```text
https://buymeacoffee.com/hostin.tech
```

Serve only the landing page folder:

```bash
scripts/serve_landing.sh
```

Then open:

```text
http://127.0.0.1:8088
```

If that port is already busy, choose another one:

```bash
PORT=8099 scripts/serve_landing.sh
```

This is the folder to expose with `cloudflared` when available.

```bash
scripts/expose_landing_cloudflared.sh
```

## Screenshots

Updated v0.3.0 captures:

- [Landing page](landing/assets/screenshots/landing-v0.3.0.jpg)
- [GitHub release page](landing/assets/screenshots/github-release-v0.3.0.jpg)

## GitHub Release

After `gh auth login -h github.com` and once a GitHub repository is configured:

```bash
GITHUB_REPO=owner/name scripts/publish_github_release.sh
```

## Source Layout

```text
BufflePlug-Analyzer/Source/
  DSP/
    EnvelopeFeatureExtractor.*
    ManualNudgeDelay.*
    TimingOffsetEstimator.*
  PluginProcessor.*
  PluginEditor.*

tests/
  DSPCoreTests.cpp

landing/
  index.html
  styles.css
  assets/
```

## Roadmap

Next highest-value work:

- Add a real capture buffer for Guide/Dub material.
- Move offset analysis to a background job over captured audio.
- Add preview/original A/B.
- Implement `Consonant Tamer Lite`.
- Add Developer ID signing and notarization.
- Pin or vendor JUCE for reproducible builds.

See `ROADMAP.md` for the full plan and `docs/build.md` for build details.
