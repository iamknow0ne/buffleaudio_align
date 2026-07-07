# V1 Host Latency Validation

Buffle Audio Align uses fixed host latency compensation for bidirectional `Nudge Dub`.

The processor reports latency equal to the maximum nudge window: `120 ms`. The audio path then delays the Dub by `120 ms + Manual Nudge`. After host plugin delay compensation, the expected audible or printed timing shift is the signed `Manual Nudge` value.

| Manual Nudge | Internal delay | Expected compensated result |
| ---: | ---: | ---: |
| `-120 ms` | `0 ms` | Dub advances `120 ms` |
| `-20 ms` | `100 ms` | Dub advances `20 ms` |
| `0 ms` | `120 ms` | Dub remains aligned |
| `+20 ms` | `140 ms` | Dub delays `20 ms` |
| `+120 ms` | `240 ms` | Dub delays `120 ms` |

## Expected Latency

| Sample rate | Reported latency |
| ---: | ---: |
| `44.1 kHz` | `5292 samples` |
| `48 kHz` | `5760 samples` |
| `96 kHz` | `11520 samples` |

Pass tolerance:

- Host-reported latency should exactly match the table when the host exposes sample latency.
- Offline rendered impulse timing should be within `1 sample` of the expected signed nudge.
- Realtime re-record tests may allow up to `1 audio block` tolerance if the host prints through live routing.

## Required Matrix

| Host | Format | Required checks |
| --- | --- | --- |
| Standalone | App | Launch, process Dub input, move `Nudge Dub` through `-120..+120 ms`. |
| Logic Pro or GarageBand | AU | Scan/load, save/reopen, confirm signed parameter recall. |
| Reaper | AU | Route Guide sidechain, render click timing at negative/zero/positive nudge. |
| Reaper | VST3 | Same render timing and signed recall checks. |
| Ableton Live | VST3 | Scan/load, sidechain if available, freeze/export timing checks. |

## Local Preflight

```bash
git status --short --branch
cmake --build build/cmake-debug --target BuffleAlignDSPTests --config Debug --parallel
ctest --test-dir build/cmake-debug --output-on-failure
cmake --build build/cmake-debug --config Debug --parallel
scripts/build_and_package_macos.sh
```

Bundle and package checks:

```bash
codesign --verify --deep --strict --verbose=2 "dist/stage/Buffle Audio Align.app"
codesign --verify --deep --strict --verbose=2 "dist/stage/Buffle Audio Align.vst3"
codesign --verify --deep --strict --verbose=2 "dist/stage/Buffle Audio Align.component"

pkgutil --payload-files dist/BuffleAudioAlign-0.3.0-macOS.pkg | rg '(^|/)\._|\.DS_Store'
unzip -l dist/BuffleAudioAlign-0.3.0-macOS-bundles.zip | rg '(^|/)\._|\.DS_Store'
```

The `rg` commands pass only when they print no matches.

## Current Local Evidence

On July 7, 2026, the staged AU component from commit `59dd173` was copied to the user Audio Units folder and validated outside the Codex sandbox with:

```bash
auval -v aufx BfAl BfAu
```

Result: `AU VALIDATION SUCCEEDED.` The run confirmed manufacturer `Buffle Audio`, AudioUnit name `Buffle Audio Align`, version `0.3.0`, published parameters, custom UI, format tests, and render tests. See [healthcheck-2026-07-07-au-validation.md](healthcheck-2026-07-07-au-validation.md).

`pluginval` is not available on this machine's `PATH`, so VST3 validation remains open until pluginval is installed or an equivalent VST3 host scan/load record is captured.

## AU Validation

```bash
mkdir -p "$HOME/Library/Audio/Plug-Ins/Components"
ditto --noextattr --noqtn \
  "dist/stage/Buffle Audio Align.component" \
  "$HOME/Library/Audio/Plug-Ins/Components/Buffle Audio Align.component"

killall -9 AudioComponentRegistrar 2>/dev/null || true
auval -a | rg -i 'Buffle|Align|BfAu|BfAl'
auval -v aufx BfAl BfAu
```

Pass criteria:

- `auval -a` lists Buffle Audio Align.
- `auval -v aufx BfAl BfAu` exits successfully.
- AU hosts instantiate the plugin and recall signed `Nudge Dub` values.

## VST3 Validation

```bash
mkdir -p "$HOME/Library/Audio/Plug-Ins/VST3"
ditto --noextattr --noqtn \
  "dist/stage/Buffle Audio Align.vst3" \
  "$HOME/Library/Audio/Plug-Ins/VST3/Buffle Audio Align.vst3"

PLUGINVAL="/path/to/pluginval"
"$PLUGINVAL" \
  --validate "$HOME/Library/Audio/Plug-Ins/VST3/Buffle Audio Align.vst3" \
  --strictness-level 10
```

Pass criteria:

- VST3 scans and instantiates.
- Host-reported latency matches the active sample rate.
- Signed `Nudge Dub` automation, save, and reload work across `-120..+120 ms`.

## Timing Procedure

For each host, format, sample rate, and buffer size:

1. Create a fresh session.
2. Import a Guide click and an aligned Dub click.
3. Insert Buffle Audio Align on the Dub track.
4. Route Guide to the plugin sidechain when the host supports it.
5. Set Preview Mode to `Aligned`, Consonant Tamer to `0%`, Audition to `100%`.
6. Render the processed Dub at `-120`, `-20`, `0`, `+20`, and `+120 ms`.
7. Measure printed click position against the original Dub click.
8. Save, close, reopen, and confirm signed nudge recall.
9. Automate `Nudge Dub` through negative, zero, and positive values, then render.

Expected printed deltas match the signed nudge values. Record any sidechain or plugin delay compensation limitation as a host-specific finding.

## V1 Pass Gate

Host latency validation passes for V1 when:

- AU validation passes.
- VST3 pluginval or equivalent host scan passes.
- Reaper AU and VST3 print timing correctly.
- Logic Pro or GarageBand AU passes load and recall smoke.
- Ableton Live VST3 passes load and timing smoke.
- At least one sidechain-capable host confirms Guide routing and signed Apply Nudge.
- Any host-specific limitations are documented before release notes are finalized.
