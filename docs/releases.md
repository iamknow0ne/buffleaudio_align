# Release Inventory

This repo publishes macOS preview builds through GitHub Releases.

For hands-on feedback, use the [V1 tester guide](v1-tester-guide.md). The bundle zip is the preferred preview download today; the `.pkg` is available for testing, but is not yet Developer ID signed or notarized.

For signed nudge and host plugin delay compensation proof, use the [host latency validation matrix](validation-host-latency.md).

The project version lives in [`../VERSION`](../VERSION). Release scripts read that file by default, and still allow `VERSION=x.y.z` for one-off previews.

Before publishing post-`v0.3.0` work, bump [`../VERSION`](../VERSION), rebuild packages, and tag the new version. The publish script refuses existing release tags, so do not reuse `v0.3.0` for a new artifact set.

## Published Releases

| Version | Status | GitHub release | Assets |
| --- | --- | --- | --- |
| `v0.4.0` | Latest macOS preview | https://github.com/iamknow0ne/buffleaudio_align/releases/tag/v0.4.0 | `BuffleAudioAlign-0.4.0-macOS.pkg`, `BuffleAudioAlign-0.4.0-macOS-bundles.zip`, `BuffleAudioAlign-0.4.0-SHA256SUMS.txt` |
| `v0.3.0` | Historical macOS preview | https://github.com/iamknow0ne/buffleaudio_align/releases/tag/v0.3.0 | `BuffleAudioAlign-0.3.0-macOS.pkg`, `BuffleAudioAlign-0.3.0-macOS-bundles.zip` |
| `v0.2.0` | Historical preview | https://github.com/iamknow0ne/buffleaudio_align/releases/tag/v0.2.0 | `BuffleAudioAlign-0.2.0-macOS.pkg`, `BuffleAudioAlign-0.2.0-macOS-bundles.zip` |

## Verification Commands

```bash
gh release list --repo iamknow0ne/buffleaudio_align --limit 20
gh release view v0.4.0 --repo iamknow0ne/buffleaudio_align --json url,tagName,name,assets
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
pkgutil --payload-files "dist/BuffleAudioAlign-$(cat VERSION)-macOS.pkg" | rg '(^|/)\._|\.DS_Store'
unzip -l "dist/BuffleAudioAlign-$(cat VERSION)-macOS-bundles.zip" | rg '(^|/)\._|\.DS_Store'
codesign --verify --deep --strict --verbose=2 "dist/stage/Buffle Audio Align.app"
codesign --verify --deep --strict --verbose=2 "dist/stage/Buffle Audio Align.vst3"
codesign --verify --deep --strict --verbose=2 "dist/stage/Buffle Audio Align.component"
pkgutil --check-signature "dist/BuffleAudioAlign-$(cat VERSION)-macOS.pkg"
spctl -a -vv -t install "dist/BuffleAudioAlign-$(cat VERSION)-macOS.pkg"
shasum -a 256 "dist/BuffleAudioAlign-$(cat VERSION)-macOS.pkg" "dist/BuffleAudioAlign-$(cat VERSION)-macOS-bundles.zip"
```

V1 also needs at least one clean-account install smoke and the DAW latency matrix. Local AU validation passed on July 7, 2026, and strict VST3 `pluginval` passed on July 8, 2026; see [healthcheck-2026-07-07-au-validation.md](healthcheck-2026-07-07-au-validation.md) and [healthcheck-2026-07-08-vst3-pluginval.md](healthcheck-2026-07-08-vst3-pluginval.md).

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
`RELEASE_MODE=full` uploads both artifacts plus `BuffleAudioAlign-X.Y.Z-SHA256SUMS.txt` and requires the package hygiene gate to pass.

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

## Published v0.4.0 Checksums

```text
f58dbdf937cee63680d04cd3b807783b5c8a204416e7f7da51f36b7a33afdc92  BuffleAudioAlign-0.4.0-macOS.pkg
088c0e5076b4131364eba6bf127c4996aa945620130e4a44368309768168ff90  BuffleAudioAlign-0.4.0-macOS-bundles.zip
```

## Published v0.3.0 Checksums

```text
9d1d2e46401960f98c3c3511a547ddf4f205c87f0bdfc8d4d83d82dc7890ca16  BuffleAudioAlign-0.3.0-macOS.pkg
fdd34cbd5be39d76fdad46df1a2e5e3f907617fec4325ce127a37917e6c5934c  BuffleAudioAlign-0.3.0-macOS-bundles.zip
```

## Local Artifacts

The ignored `dist/` folder is local build output and may contain staged bundles, `pkgroot`, and top-level package/archive artifacts from the most recent package run. Do not treat local `dist/` contents as the public source of truth; GitHub Releases and completed release evidence records are the public artifact record.

## Signing And Package State

The staged bundles are ad-hoc signed for local verification. The installer packages are not Developer ID Installer signed or notarized yet.

The build script now repairs package payload metadata and refuses to continue when `pkgutil --payload-files` reports AppleDouble `._*` or `.DS_Store` entries in the installer payload. The publish script repeats the payload hygiene gate for `pkg` and `full` release modes. Use `RELEASE_MODE=zip` when the package is intentionally held back.

Current release blockers before a broad V1 installer:

- Developer ID Application and Developer ID Installer identities are not available in this local keychain.
- Fresh package and archive hygiene evidence is required for each installer candidate.
- The bundle `.zip` remains the preferred preview download until signing, notarization, and clean-account install smoke pass.
- AU validation and strict VST3 `pluginval` have current local evidence; DAW timing checks and a clean-account install smoke still need to be run.

Preview releases should be treated as GitHub prereleases until those gates pass.
