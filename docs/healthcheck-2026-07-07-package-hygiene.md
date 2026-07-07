# Healthcheck - Package Hygiene

Date: 2026-07-07

## Scope

This slice hardens local macOS package generation so the `.pkg` payload no longer ships AppleDouble `._*` or `.DS_Store` metadata entries.

## Change

- `scripts/build_and_package_macos.sh` now runs a package repair pass after `pkgbuild`.
- The repair pass expands the package, extracts the payload, deletes AppleDouble and `.DS_Store` entries, regenerates the BOM, rebuilds the Payload, and flattens the package again.
- The existing hard hygiene gate still runs after repair and refuses dirty package payloads.

## Local Verification

```bash
CLEAN_DIST=1 scripts/build_and_package_macos.sh
pkgutil --payload-files dist/BuffleAudioAlign-0.3.0-macOS.pkg | rg '(^|/)\._|\.DS_Store'
unzip -l dist/BuffleAudioAlign-0.3.0-macOS-bundles.zip | rg '(^|/)\._|\.DS_Store'
codesign --verify --deep --strict --verbose=2 "dist/stage/Buffle Audio Align.app"
codesign --verify --deep --strict --verbose=2 "dist/stage/Buffle Audio Align.vst3"
codesign --verify --deep --strict --verbose=2 "dist/stage/Buffle Audio Align.component"
pkgutil --check-signature dist/BuffleAudioAlign-0.3.0-macOS.pkg
shasum -a 256 dist/BuffleAudioAlign-0.3.0-macOS.pkg dist/BuffleAudioAlign-0.3.0-macOS-bundles.zip
```

Results:

- Package payload metadata check: no matches.
- Bundle zip metadata check: no matches.
- Standalone app, VST3, and AU staged bundles pass strict ad-hoc `codesign` verification.
- Package signature: `Status: no signature`, expected until Developer ID Installer signing is available.
- Local artifact checksums:

```text
8248fdac691e4f3e30bd829a2fabb66780ad4ffdc6a5d7f9523297c3b4bff42b  dist/BuffleAudioAlign-0.3.0-macOS.pkg
c033214b5aa59dbc12584f7cf1bdfce619edc024b5ad7981837c19316db70a4d  dist/BuffleAudioAlign-0.3.0-macOS-bundles.zip
```

## Remaining V1 Risk

- These are local artifacts from current `main`, not a new public GitHub release.
- Developer ID Application signing, Developer ID Installer signing, notarization, stapling, `spctl`, clean-account install smoke, AU validation, pluginval/VST3 host validation, and DAW host latency validation remain V1 release gates.
