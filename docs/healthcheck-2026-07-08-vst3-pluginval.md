# VST3 Pluginval Healthcheck - 2026-07-08

This records local VST3 validation for the current Debug build. It is not a DAW host timing-matrix result and does not replace Logic/Reaper/Ableton validation.

## Scope

- Source: July 8, 2026 local source tree with Articulation Risk strip changes.
- Tested artifact: `build/cmake-debug/BuffleAlign_artefacts/Debug/VST3/Buffle Audio Align.vst3`
- Validator: `/Applications/pluginval.app/Contents/MacOS/pluginval`
- Strictness: `10`
- Command executed outside the Codex sandbox.

## Command

```bash
/Applications/pluginval.app/Contents/MacOS/pluginval \
  --validate "build/cmake-debug/BuffleAlign_artefacts/Debug/VST3/Buffle Audio Align.vst3" \
  --strictness-level 10
```

## Result

`pluginval` found one VST3:

```text
Buffle Audio: Buffle Audio Align v0.3.0
```

It completed scan, cold/warm open, plugin info, editor, editor-while-processing, audio processing, non-releasing audio processing, state restoration, automation, editor automation, automatable parameters, parameter checks, background thread state, parameter thread safety, bus listing, all-bus enable, non-main bus disable, default layout restore, and parameter fuzzing.

Final validator result:

```text
SUCCESS
```

Exit status: `0`.

## Notes

- Running `pluginval` inside the Codex sandbox aborted without useful output; running outside the sandbox succeeded.
- The Debug validator run printed JUCE assertions from `juce_AudioProcessor.cpp` during cold open and parameter thread safety, but these did not fail the strictness-level-10 validator result.
- The VST3 validator sub-step was skipped because `pluginval` did not have a separate Steinberg validator path configured.

## Still Open For V1

- DAW host latency matrix: Logic/GarageBand AU, Reaper AU/VST3, Ableton VST3, and Standalone sanity.
- Clean macOS account install smoke.
- Developer ID Application and Installer signing.
- Notarization and stapling.
