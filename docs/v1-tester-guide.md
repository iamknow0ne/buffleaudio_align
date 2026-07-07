# Buffle Audio Align V1 Tester Guide

Use this guide when testing the current macOS developer preview and sending feedback toward V1.

## Preferred Download

Use the bundle archive first:

```text
BuffleAudioAlign-0.3.0-macOS-bundles.zip
```

The `.pkg` exists for local preview testing, but it is not Developer ID signed or notarized yet. Treat the package as rough release-lane evidence, not the preferred tester install path.

## Install Paths

After unzipping the bundle archive:

- Standalone app: open `Buffle Audio Align.app` directly from the staged folder, or move it into `/Applications`.
- VST3: copy `Buffle Audio Align.vst3` to `~/Library/Audio/Plug-Ins/VST3/`.
- AU: copy `Buffle Audio Align.component` to `~/Library/Audio/Plug-Ins/Components/`.

Because this is an unsigned developer preview, macOS Gatekeeper may ask you to explicitly allow the app or plugin. For broad distribution, V1 still needs Developer ID signing, notarization, and a clean-account install smoke.

## Smoke Test

1. Open the Standalone app first if DAW sidechain routing is unclear.
2. In a DAW, insert Align on the Dub, double, backing vocal, ADR, or stack layer.
3. Route the Guide vocal into the plugin sidechain when the host supports it.
4. Play a short phrase and watch the session flow rail.
5. Confirm the UI moves from route/listen into locked confidence before trusting numbers.
6. Check the signed offset and suggested `Delay Dub` or `Advance Dub` action.
7. Click Apply Nudge only when the button shows a concrete signed action.
8. A/B `Original`, `Aligned`, and `Diff` before committing the move.
9. Try one `Stack Role`: Double Tight, Choir Natural, Rap Stack, or ADR Loose.
10. Raise `Consonant Tamer` gradually and watch Naturalness Risk move between `Natural`, `Check Diff`, and `Too Much`.
11. Use `Diff` before printing whenever the guardrail says `Check Diff` or `Too Much`.
12. Use `Copy Report` and include that text in feedback.

## Useful Test Material

Please try at least one of these cases:

- Tight double with a small timing flam.
- Loose double that should not become sterile.
- Rap stack with sharp consonants.
- Choir or harmony layer where natural spread matters.
- ADR dub against a guide line.
- Weak guide or noisy dub, to verify Align refuses fake certainty.

## Feedback Checklist

Send:

- macOS version and Apple Silicon or Intel.
- DAW name/version and plugin format tested: Standalone, AU, or VST3.
- Install path used: bundle zip or `.pkg`.
- Vocal case: double, harmony, rap stack, ADR, backing vocal, or other.
- Whether the Guide sidechain was detected.
- Confidence value, signed offset, and suggested nudge direction.
- Whether Apply Nudge moved the Dub in the expected direction.
- Whether `Diff` sounded useful, too subtle, or misleading.
- Stack Role used and whether it helped.
- Naturalness Risk state and whether it matched what you heard.
- Consonant Tamer setting and any artifacts heard.
- Paste the `Copy Report` output.
- Attach a screenshot or short audio before/after if possible.

## Known Preview Limits

- `Arm Listen`, `Check Timing`, and `Preview Align` are still workflow/checklist controls, not a complete captured phrase editor.
- Bidirectional nudge uses fixed host latency compensation and still needs DAW host validation.
- `Guide Blend` and `Stereo Focus` are preview/monitoring shape controls until deeper stereo DSP lands.
- `Changed Material` is an overall processed-vs-original preview meter, not per-feature removed-material solo yet.
- The installer is not Developer ID signed or notarized.
