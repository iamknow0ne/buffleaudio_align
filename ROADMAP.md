# Buffle Audio - Align Roadmap

This roadmap reflects the refreshed direction for the project after the first modernization pass. The old plan was ambitious and directionally useful, but the codebase is still early, so the next milestones focus on making a trustworthy vocal-stack cleanup tool before adding full offline warping, ARA, or MIDI groove features.

## Milestone 0 - Project Foundation

Status: mostly complete. The remaining foundation work is release metadata/signing and deeper host validation.

- Replace the starter JUCE editor with the Buffle Audio - Align cockpit. Done.
- Add persistent parameters with stable IDs. Done.
- Add state save/restore. Done.
- Show live Guide/Dub levels and a match-confidence surface. Done.
- Add optional Guide sidechain bus and audible manual nudge delay. Done.
- Document the current dependency blocker and architecture direction. Done.

## Milestone 1 - Build Reproducibility

Status: partially complete. The new CMake path builds locally with the JUCE checkout at `/Users/hostin/vibecoding/waveform-visualizer/JUCE`.

- Add CMake build for Standalone, VST3, and AU. Done.
- Add macOS packaging script for `.app`, `.vst3`, `.component`, and `.pkg`. Done.
- Pin JUCE in the repo or keep documenting `JUCE_PATH`.
- Remove hardcoded `/Users/hostin/Downloads/JUCE/modules` paths. Done in the `.jucer`; generated Xcode files still need regeneration if they are used.
- Keep VST3, AU, and Standalone targets.
- Set real plugin identity: manufacturer, bundle ID, plugin code, version, and website.
- Add Developer ID signing and notarization before public distribution.

## Milestone 2 - Align Prep v0.2

Goal: create the first usable vocal-stack cleanup workflow.

- Add Guide/Dub capture or sidechain monitoring.
- Extract RMS/onset envelopes from each source. Initial standalone DSP module and tests added.
- Estimate a global timing offset with cross-correlation. Initial history-based estimator added.
- Display suggested nudge in milliseconds. Initial UI readout added.
- Let the user apply or preview manual nudge/tightness. Manual nudge delay is implemented; automatic apply remains.
- Keep audio-thread work allocation-free and move analysis to a background job.

## Milestone 3 - Consonant Tamer Lite

Goal: deliver the first differentiated feature.

- Detect likely consonant regions with transient/high-frequency/envelope heuristics.
- Add fade-safe attenuation controlled by `Consonant Level`.
- Add an audition mode for removed/attenuated material.
- Preserve breaths and strong attacks unless the user explicitly tightens cleanup.

## Milestone 4 - Testable DSP Core

Goal: move alignment math into isolated C++ modules.

Recommended modules:

- `AudioClip`
- `CaptureBuffer`
- `EnvelopeFeatureExtractor`
- `TimingOffsetEstimator`
- `DtwAligner`
- `WarpMap`
- `AlignmentPreviewRenderer`
- `AlignmentEngine`

Initial tests:

- Silence produces zero/non-NaN envelope.
- Impulse lands in expected analysis frame.
- Equal sequences produce a diagonal DTW path.
- Shifted synthetic clicks recover the known offset.
- Tightness `0` is identity, `1` is full warp, `0.5` is bounded interpolation.

## Milestone 5 - Full Alignment Engine

Goal: replace nudge-only cleanup with constrained offline time warping.

- Implement constrained DTW with a Sakoe-Chiba-style band and slope limits.
- Convert DTW paths into smoothed monotonic warp maps.
- Render an aligned preview with interpolation first.
- Evaluate Rubber Band, PSOLA, or another stretcher only after the warp map is proven.
- Preserve consonants and transients during stretch where possible.

## Milestone 6 - DAW Workflow

Goal: make the plugin practical in real sessions.

- Add sidechain Guide input or a clear two-pass capture workflow.
- Add preview/original A/B.
- Add region status and error states.
- Add export/print strategy.
- Evaluate ARA only after the non-ARA workflow is useful.

## Milestone 7 - Groove Mode

Goal: explore the creative MIDI/slice idea after alignment is credible.

- Segment vowels/consonants more robustly.
- Add MIDI-triggered slice playback.
- Add root note and slice map controls.
- Explore vowel quantization to MIDI as an experimental feature.

## Deferred Until Proven Needed

- ML phoneme recognition.
- ARA.
- AAX.
- Real-time live alignment.
- Rubber Band integration in proprietary builds without a commercial license.
