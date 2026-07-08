# Product Landing And Workflow Healthcheck - 2026-07-08

This records the pass that re-centered Buffle Audio Align presentation around the real product surface instead of development progress.

## Scope

- Reframed `landing/index.html` around vocal-stack timing, Guide/Dub routing, safe nudge, Stack Role presets, A/B preview, Consonant Tamer Lite, Phrase Health, and session reports.
- Kept the landing page focused on one current standalone app screenshot instead of GitHub/release proof screenshots.
- Updated README product preview copy to show the current standalone app surface first.
- Captured the updated landing hero at `landing/assets/screenshots/landing-product-2026-07-08.jpg`.
- Replaced "development build" wording in the About panel with "macOS preview".
- Added a pure `WorkflowState` helper and test coverage for Route, Listen, Preview, Tame, and Print rail states.
- Connected editor workflow buttons and preview/report actions to the intent-aware workflow rail.
- Promoted Phrase Health above supporting evidence strips in the app surface.

## Product Truth

- Public downloads are still the `v0.3.0` macOS preview artifacts.
- Current `main` includes newer product polish than the published package.
- The landing page only claims capabilities already represented in the app/source surface: Guide/Dub timing, safe nudge, preview modes, Stack Role presets, Stack Spread Governor v0, Consonant Tamer Lite, Breath Preservation Mask v0, Articulation Risk, Naturalness Guardrail, Phrase Health, and report handoff.

## Remaining Release Gates

- Build a fresh post-`v0.3.0` artifact set under a bumped version.
- Run clean-account install smoke.
- Complete the DAW latency validation matrix.
- Sign and notarize broader public installers when Developer ID credentials are available.
