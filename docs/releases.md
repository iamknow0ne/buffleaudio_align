# Release Inventory

This repo publishes macOS developer preview builds through GitHub Releases.

## Published Releases

| Version | Status | GitHub release | Assets |
| --- | --- | --- | --- |
| `v0.3.0` | Latest | https://github.com/iamknow0ne/buffleaudio_align/releases/tag/v0.3.0 | `BuffleAudioAlign-0.3.0-macOS.pkg`, `BuffleAudioAlign-0.3.0-macOS-bundles.zip` |
| `v0.2.0` | Historical preview | https://github.com/iamknow0ne/buffleaudio_align/releases/tag/v0.2.0 | `BuffleAudioAlign-0.2.0-macOS.pkg`, `BuffleAudioAlign-0.2.0-macOS-bundles.zip` |

## Verification Commands

```bash
gh release list --repo iamknow0ne/buffleaudio_align --limit 20
gh release view v0.3.0 --repo iamknow0ne/buffleaudio_align --json url,tagName,name,assets
gh release view v0.2.0 --repo iamknow0ne/buffleaudio_align --json url,tagName,name,assets
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

## Signing State

The staged bundles are ad-hoc signed for local verification. The installer packages are not Developer ID Installer signed or notarized yet.
