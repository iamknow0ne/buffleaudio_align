# Release Inventory

This repo publishes macOS developer preview builds through GitHub Releases.

## Published Releases

| Version | Status | GitHub release | Assets |
| --- | --- | --- | --- |
| `v0.3.0` | Latest developer preview | https://github.com/iamknow0ne/buffleaudio_align/releases/tag/v0.3.0 | `BuffleAudioAlign-0.3.0-macOS.pkg`, `BuffleAudioAlign-0.3.0-macOS-bundles.zip` |
| `v0.2.0` | Historical preview | https://github.com/iamknow0ne/buffleaudio_align/releases/tag/v0.2.0 | `BuffleAudioAlign-0.2.0-macOS.pkg`, `BuffleAudioAlign-0.2.0-macOS-bundles.zip` |

## Verification Commands

```bash
gh release list --repo iamknow0ne/buffleaudio_align --limit 20
gh release view v0.3.0 --repo iamknow0ne/buffleaudio_align --json url,tagName,name,assets
gh release view v0.2.0 --repo iamknow0ne/buffleaudio_align --json url,tagName,name,assets
```

## Release Checklist

Run this before promoting a preview into a broader public release:

```bash
git status --short --branch
cmake --build build/cmake-debug --config Debug --parallel
ctest --test-dir build/cmake-debug --output-on-failure
scripts/build_and_package_macos.sh
find dist/stage dist/pkgroot \( -name '._*' -o -name '.DS_Store' \) -print
pkgutil --payload-files dist/BuffleAudioAlign-0.3.0-macOS.pkg | rg '(^|/)\._|\.DS_Store'
unzip -l dist/BuffleAudioAlign-0.3.0-macOS-bundles.zip | rg '(^|/)\._|\.DS_Store'
codesign --verify --deep --strict --verbose=2 "dist/stage/Buffle Audio Align.app"
codesign --verify --deep --strict --verbose=2 "dist/stage/Buffle Audio Align.vst3"
codesign --verify --deep --strict --verbose=2 "dist/stage/Buffle Audio Align.component"
pkgutil --check-signature dist/BuffleAudioAlign-0.3.0-macOS.pkg
spctl -a -vv -t install dist/BuffleAudioAlign-0.3.0-macOS.pkg
shasum -a 256 dist/BuffleAudioAlign-0.3.0-macOS.pkg dist/BuffleAudioAlign-0.3.0-macOS-bundles.zip
```

V1 also needs at least one clean-account install smoke, AU validation, and a VST3 host/pluginval pass.

## v0.3.0 Checksums

```text
9d1d2e46401960f98c3c3511a547ddf4f205c87f0bdfc8d4d83d82dc7890ca16  BuffleAudioAlign-0.3.0-macOS.pkg
fdd34cbd5be39d76fdad46df1a2e5e3f907617fec4325ce127a37917e6c5934c  BuffleAudioAlign-0.3.0-macOS-bundles.zip
```

## Local Artifacts

The ignored `dist/` folder currently contains:

```text
dist/BuffleAudioAlign-0.2.0-macOS-bundles.zip
dist/BuffleAudioAlign-0.2.0-macOS.pkg
dist/BuffleAudioAlign-0.3.0-macOS-bundles.zip
dist/BuffleAudioAlign-0.3.0-macOS.pkg
```

These files are build outputs and are intentionally not tracked by git. GitHub Releases are the public artifact source of truth.

## Signing And Package State

The staged bundles are ad-hoc signed for local verification. The installer packages are not Developer ID Installer signed or notarized yet.

Current release blockers before a broad V1 installer:

- Developer ID Application and Developer ID Installer identities are not available in this local keychain.
- The `.pkg` payload still needs AppleDouble `._*` cleanup before signing/notarization.
- The bundle `.zip` is the cleaner preview artifact lane today.
- AU validation, pluginval/VST3 host validation, and a clean-account install smoke still need to be run.

Preview releases should be treated as GitHub prereleases until those gates pass.
