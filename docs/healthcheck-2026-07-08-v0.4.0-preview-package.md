# v0.4.0 Preview Package Healthcheck - 2026-07-08

This records the local package and release-candidate evidence for the `v0.4.0` macOS preview.

## Scope

- Bumped repository version to `0.4.0`.
- Built Release Standalone, VST3, and AU bundles.
- Built `dist/BuffleAudioAlign-0.4.0-macOS.pkg`.
- Built `dist/BuffleAudioAlign-0.4.0-macOS-bundles.zip`.
- Generated `dist/BuffleAudioAlign-0.4.0-SHA256SUMS.txt`.
- Hardened packaging so missing Standalone, VST3, or AU artifacts fail the package lane.
- Added actual Guide Blend monitor output when a Guide sidechain is available.

## Commands

```bash
CLEAN_DIST=1 scripts/build_and_package_macos.sh
ctest --test-dir build/cmake-release --output-on-failure
pkgutil --payload-files dist/BuffleAudioAlign-0.4.0-macOS.pkg | rg '(^|/)\._|\.DS_Store' || true
unzip -l dist/BuffleAudioAlign-0.4.0-macOS-bundles.zip | rg '(^|/)\._|\.DS_Store' || true
codesign --verify --deep --strict --verbose=2 "dist/stage/Buffle Audio Align.app"
codesign --verify --deep --strict --verbose=2 "dist/stage/Buffle Audio Align.vst3"
codesign --verify --deep --strict --verbose=2 "dist/stage/Buffle Audio Align.component"
pkgutil --check-signature dist/BuffleAudioAlign-0.4.0-macOS.pkg
shasum -a 256 -c dist/BuffleAudioAlign-0.4.0-SHA256SUMS.txt
```

## Results

- `BuffleAlignDSPTests` passed in the Release CTest lane.
- Package payload hygiene check returned no AppleDouble or `.DS_Store` entries.
- Bundle zip hygiene check returned no AppleDouble or `.DS_Store` entries.
- Standalone, VST3, and AU staged bundles passed `codesign --verify --deep --strict`.
- All staged bundle `CFBundleShortVersionString` values report `0.4.0`.
- `pkgutil --check-signature` reports `Status: no signature`, expected for the current preview installer.
- SHA256 verification passed.

## Checksums

```text
f58dbdf937cee63680d04cd3b807783b5c8a204416e7f7da51f36b7a33afdc92  BuffleAudioAlign-0.4.0-macOS.pkg
088c0e5076b4131364eba6bf127c4996aa945620130e4a44368309768168ff90  BuffleAudioAlign-0.4.0-macOS-bundles.zip
```

## Remaining V1 Gates

- Developer ID Application and Installer signing.
- Notarization and staple.
- Clean-account install smoke.
- DAW latency validation matrix.
