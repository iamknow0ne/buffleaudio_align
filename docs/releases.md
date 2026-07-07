# Release Inventory

This repo publishes macOS developer preview builds through GitHub Releases.

For hands-on feedback, use the [V1 tester guide](v1-tester-guide.md). The bundle zip is the preferred preview lane today; the `.pkg` remains a rough local preview artifact until signing, notarization, and clean-account install smoke pass.

For signed nudge and host plugin delay compensation proof, use the [host latency validation matrix](validation-host-latency.md).

The project version lives in [`../VERSION`](../VERSION). Release scripts read that file by default, and still allow `VERSION=x.y.z` for one-off previews.

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

## Publish Modes

Preview releases should prefer the clean bundle archive:

```bash
RELEASE_MODE=zip GITHUB_REPO=iamknow0ne/buffleaudio_align scripts/publish_github_release.sh
```

Installer-inclusive release candidates must use:

```bash
RELEASE_MODE=full GITHUB_REPO=iamknow0ne/buffleaudio_align scripts/publish_github_release.sh
```

`RELEASE_MODE=zip` uploads only `BuffleAudioAlign-X.Y.Z-macOS-bundles.zip`.
`RELEASE_MODE=pkg` uploads only the installer and requires clean package payload hygiene.
`RELEASE_MODE=full` uploads both artifacts and requires the package hygiene gate to pass.

The publish script refuses a dirty working tree unless `ALLOW_DIRTY=1` is set, refuses existing release tags, verifies bundle archive metadata, and creates draft prereleases by default. Use `DRAFT_RELEASE=0 PRERELEASE=0` only for a fully validated V1.

Use [release-evidence-template.md](release-evidence-template.md) for each V1 release candidate.

## V1 Promotion Gates

Do not call a build V1 until these gates have current evidence:

- Clean `dist/` rebuild for the release candidate.
- Debug and Release builds pass.
- CTest passes.
- Bundle zip contains no AppleDouble `._*` or `.DS_Store` entries.
- `.pkg` payload contains no AppleDouble `._*` or `.DS_Store` entries.
- Standalone, VST3, and AU bundles pass `codesign --verify --deep --strict`.
- Developer ID Application signing is available and applied.
- Developer ID Installer signing is available and applied.
- Notarization succeeds and the installer is stapled.
- `spctl` install assessment passes.
- Clean macOS account install smoke passes.
- AU validation passes.
- VST3/pluginval or host scan passes.
- DAW host matrix in [validation-host-latency.md](validation-host-latency.md) passes for at least Logic/GarageBand AU, Reaper AU/VST3, Ableton VST3, and Standalone sanity.
- Landing, README, release notes, screenshots, checksums, and tester guide match the shipped behavior.

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

The build and publish scripts now refuse to continue when `pkgutil --payload-files` reports AppleDouble `._*` or `.DS_Store` entries in the installer payload. Use `RELEASE_MODE=zip` when the package is intentionally held back.

Current release blockers before a broad V1 installer:

- Developer ID Application and Developer ID Installer identities are not available in this local keychain.
- The `.pkg` payload still needs AppleDouble `._*` cleanup before signing/notarization.
- The bundle `.zip` is the cleaner preview artifact lane today.
- AU validation, pluginval/VST3 host validation, and a clean-account install smoke still need to be run.

Preview releases should be treated as GitHub prereleases until those gates pass.
