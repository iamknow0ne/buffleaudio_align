# Buffle Audio Align V1 Tester Guide

Use this guide when testing the current macOS developer preview and sending feedback toward V1.

## Version Status

| Lane | What it means |
| --- | --- |
| `v0.3.0` packaged preview | Current public GitHub release artifacts for Standalone, AU, VST3, and rough `.pkg` preview. |
| Current `main` source | Post-`v0.3.0` V1-preview polish, including Trust Diagnostics and Tamer audition work, pending fresh release artifacts. |
| Next preview target | Fresh zip-first artifacts after local build, DSP tests, release hygiene, and host-validation evidence are refreshed. |

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
8. A/B `Original`, `Aligned`, `All Diff`, and `Tamer` before committing the move.
9. Try one `Stack Role`: Double Tight, Choir Natural, Rap Stack, or ADR Loose.
10. Raise `Consonant Tamer` gradually and watch Naturalness Risk move between `Natural`, `Check Diff`, and `Too Much`.
11. Use `Diff` before printing whenever the guardrail says `Check Diff` or `Too Much`.
12. Use `Tamer` when judging whether Consonant Tamer is removing useful consonant clutter or audible performance detail.
13. Use `Copy Report` and include that text in feedback.

## Trust Diagnostics

The Trust Meter should explain why timing is available or unavailable. During testing, try to capture at least three of these reason codes in `Copy Report`:

| Scenario | Expected reason code | Expected behavior |
| --- | --- | --- |
| No sidechain Guide routed | `ROUTE_GUIDE` | No offset or nudge number is trusted. |
| Guide present but too low | `GUIDE_TOO_QUIET` | App asks for more Guide level instead of inventing timing. |
| Dub/main input too low | `DUB_TOO_QUIET` | App asks for more Dub level before nudge/tamer decisions. |
| Both lanes audible but unstable | `LISTENING_FOR_CONFIDENCE` | App keeps listening and hides stale timing numbers. |
| Timing is stable with no useful correction | `LOCKED_NO_NUDGE` | App reports locked timing without pushing an unnecessary nudge. |
| Dub is early | `DUB_EARLY_SAFE_DELAY` | App offers a confidence-gated `Delay Dub` move. |
| Dub is late | `DUB_LATE_SAFE_ADVANCE` | App offers a confidence-gated `Advance Dub` move. |

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
- `Phrase health`, `Trust reason`, and `Trust advice` lines from `Copy Report`.
- Confidence value, signed offset, and suggested nudge direction.
- Whether Apply Nudge moved the Dub in the expected direction.
- Whether `Diff` sounded useful, too subtle, or misleading.
- Whether `Tamer` isolated consonant cleanup clearly from the full timing/difference path.
- Stack Role used and whether it helped.
- Naturalness Risk state and whether it matched what you heard.
- Consonant Tamer setting and any artifacts heard.
- Paste the `Copy Report` output.
- Attach a screenshot or short audio before/after if possible.

## Known Preview Limits

- `Arm Listen`, `Check Timing`, and `Preview Align` are still workflow/checklist controls, not a complete captured phrase editor.
- Bidirectional nudge uses fixed host latency compensation and still needs DAW host validation.
- `Guide Blend` and `Stereo Focus` are preview/monitoring shape controls until deeper stereo DSP lands.
- `All Diff` is the overall processed-vs-original preview delta; `Tamer` isolates the current Consonant Tamer reduction only.
- The installer is not Developer ID signed or notarized.
