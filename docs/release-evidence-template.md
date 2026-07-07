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
