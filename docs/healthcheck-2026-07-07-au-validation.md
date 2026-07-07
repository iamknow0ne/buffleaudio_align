# AU Validation Healthcheck - 2026-07-07

This records local Audio Unit validation for the current source artifact. It is not a claim that the older public GitHub `v0.3.0` release asset has been reissued.

## Scope

- Source commit: `59dd173`
- Version file: `0.3.0`
- Tested artifact: `dist/stage/Buffle Audio Align.component`
- Installed validation path: `$HOME/Library/Audio/Plug-Ins/Components/Buffle Audio Align.component`
- AU identity: `aufx` / `BfAl` / `BfAu`
- `auval`: `/usr/bin/auval`
- `pluginval`: not available on `PATH`

## Commands

```bash
codesign --verify --deep --strict --verbose=2 "dist/stage/Buffle Audio Align.component"
mkdir -p "$HOME/Library/Audio/Plug-Ins/Components"
ditto --noextattr --noqtn \
  "dist/stage/Buffle Audio Align.component" \
  "$HOME/Library/Audio/Plug-Ins/Components/Buffle Audio Align.component"
xattr -c "$HOME/Library/Audio/Plug-Ins/Components/Buffle Audio Align.component" \
  "$HOME/Library/Audio/Plug-Ins/Components/Buffle Audio Align.component/Contents/Info.plist" \
  "$HOME/Library/Audio/Plug-Ins/Components/Buffle Audio Align.component/Contents/MacOS/Buffle Audio Align" \
  "$HOME/Library/Audio/Plug-Ins/Components/Buffle Audio Align.component/Contents/PkgInfo"
auval -v aufx BfAl BfAu
```

`auval` must be run outside the Codex filesystem sandbox on this machine. Inside the sandbox, `AudioComponentRegistrar` can register the component but sandboxed `auval` still reports that it cannot find the component.

## Results

- Staged AU component: `codesign --verify --deep --strict` passed.
- `AudioComponentRegistrar` log: added `aufx/BfAl/BfAu`.
- Targeted AU validation: `auval -v aufx BfAl BfAu` passed.
- Manufacturer string: `Buffle Audio`.
- AudioUnit name: `Buffle Audio Align`.
- Component version: `0.3.0 (0x300)`.
- Open, property, parameter, format, custom UI, and render tests: passed.

Key terminal result:

```text
AU VALIDATION SUCCEEDED.
```

## Local Artifact Checksums

These checksums are for the current local artifacts generated from commit `59dd173`, not the already-published GitHub release assets.

```text
8248fdac691e4f3e30bd829a2fabb66780ad4ffdc6a5d7f9523297c3b4bff42b  dist/BuffleAudioAlign-0.3.0-macOS.pkg
c033214b5aa59dbc12584f7cf1bdfce619edc024b5ad7981837c19316db70a4d  dist/BuffleAudioAlign-0.3.0-macOS-bundles.zip
```

## Still Open For V1

- `pluginval` or equivalent VST3 host scan/load validation.
- Logic/GarageBand AU load and recall smoke.
- Reaper AU/VST3 signed-nudge timing renders.
- Ableton VST3 scan/load and timing smoke.
- Clean macOS account install smoke.
- Developer ID Application signing.
- Developer ID Installer signing.
- Notarization and stapling.
