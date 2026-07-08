# Release Evidence Template

Copy this file to `docs/release-evidence-vX.Y.Z.md` for each V1 release candidate.

## Release

- Version:
- Tag:
- Date:
- Machine:
- macOS:
- Apple Silicon or Intel:
- Commit:
- Release mode: `zip`, `pkg`, or `full`

## Build

```bash
git status --short --branch
cmake --build build/cmake-debug --target BuffleAlignDSPTests --config Debug --parallel
ctest --test-dir build/cmake-debug --output-on-failure
cmake --build build/cmake-debug --config Debug --parallel
CLEAN_DIST=1 scripts/build_and_package_macos.sh
```

Result:

## Artifact Hygiene

```bash
unzip -l dist/BuffleAudioAlign-X.Y.Z-macOS-bundles.zip | rg '(^|/)\._|\.DS_Store'
pkgutil --payload-files dist/BuffleAudioAlign-X.Y.Z-macOS.pkg | rg '(^|/)\._|\.DS_Store'
```

Result:

## Signing

```bash
codesign --verify --deep --strict --verbose=2 "dist/stage/Buffle Audio Align.app"
codesign --verify --deep --strict --verbose=2 "dist/stage/Buffle Audio Align.vst3"
codesign --verify --deep --strict --verbose=2 "dist/stage/Buffle Audio Align.component"
pkgutil --check-signature dist/BuffleAudioAlign-X.Y.Z-macOS.pkg
spctl -a -vv -t install dist/BuffleAudioAlign-X.Y.Z-macOS.pkg
```

Result:

## Notarization

- Notary profile:
- Notary request ID:
- Staple result:

## AU Validation

```bash
auval -a | rg -i 'Buffle|Align|BfAu|BfAl'
auval -v aufx BfAl BfAu
```

Result:

## VST3 Validation

```bash
pluginval --validate "$HOME/Library/Audio/Plug-Ins/VST3/Buffle Audio Align.vst3" --strictness-level 10
```

Result:

## Host Latency Matrix

Use [validation-host-latency.md](validation-host-latency.md).

| Host | Format | Sample rate | Buffer | Reported latency | Printed timing | Recall | Result |
| --- | --- | ---: | ---: | ---: | ---: | --- | --- |
| Standalone | App | | | | | | |
| Logic/GarageBand | AU | | | | | | |
| Reaper | AU | | | | | | |
| Reaper | VST3 | | | | | | |
| Ableton Live | VST3 | | | | | | |

## Trust Diagnostics Acceptance

Capture these from the app UI and `Copy Report`.

| Scenario | Expected label | Expected reason | Actual report text | Result |
| --- | --- | --- | --- | --- |
| No sidechain Guide | Route Guide | `ROUTE_GUIDE` | | |
| Quiet Guide | Guide too quiet | `GUIDE_TOO_QUIET` | | |
| Quiet Dub | Dub too quiet | `DUB_TOO_QUIET` | | |
| Audible but unstable | Listening for confidence | `LISTENING_FOR_CONFIDENCE` | | |
| Stable/no correction | Locked - no nudge | `LOCKED_NO_NUDGE` | | |
| Dub early | Dub early - safe delay | `DUB_EARLY_SAFE_DELAY` | | |
| Dub late | Dub late - safe advance | `DUB_LATE_SAFE_ADVANCE` | | |

## Articulation Strip Acceptance

| Scenario | Expected visual result | Result | Notes |
| --- | --- | --- | --- |
| Tight double, modest tamer | `Clean` or low `Watch consonants` | Pending | Confirm with Tamer audition. |
| Rap stack with unmatched attacks | `Watch consonants` or `Collision risk` | Pending | Should correlate with audible clutter, not claim exact phonemes. |
| Missing Guide or unreliable confidence | `Listening` | Pending | Should avoid fake certainty. |
| High tamer / high consonant removal | Elevated strip score | Pending | Check Naturalness Guardrail and All Diff too. |

## Clean Account Install

- Account:
- Install path:
- Gatekeeper behavior:
- Launch/scan result:

## Checksums

```bash
shasum -a 256 dist/BuffleAudioAlign-X.Y.Z-macOS.pkg dist/BuffleAudioAlign-X.Y.Z-macOS-bundles.zip
```

Result:

## Publish

Preview zip-only draft:

```bash
RELEASE_MODE=zip GITHUB_REPO=iamknow0ne/buffleaudio_align scripts/publish_github_release.sh
```

V1 full release candidate draft:

```bash
RELEASE_MODE=full GITHUB_REPO=iamknow0ne/buffleaudio_align scripts/publish_github_release.sh
```

Result:
