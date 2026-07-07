# Buffle Audio Align Healthcheck - 2026-07-07 Session Flow Polish

## Scope

This healthcheck covers the post-`v0.3.0` session-flow and documentation polish slice:

- Editor `Next Best Move` card for route/listen/apply/A-B/print guidance.
- Workflow rail upgraded with dynamic per-step state details.
- Apply Nudge text changed to explicit `Delay Dub` / `Advance Dub` actions.
- `Changed Material` wording clarified as overall preview change, not per-feature solo.
- Stack Role selector expanded to full role names.
- README, landing page, release notes, changelog, roadmap, and release docs updated for zip-first testing and V1 tester workflow.
- New `docs/v1-tester-guide.md` for installer paths, DAW smoke tests, feedback checklist, and known preview limits.

## Verification Commands

```bash
cmake --build build/cmake-debug --config Debug --parallel
ctest --test-dir build/cmake-debug --output-on-failure
bash -n scripts/build_and_package_macos.sh scripts/publish_github_release.sh scripts/expose_landing_cloudflared.sh scripts/serve_landing.sh
git diff --check
```

## Still Open For V1

- The session rail and checklist are UI guidance, not a captured phrase editor yet.
- Bidirectional nudge still needs pluginval, AU validation, and DAW host latency tests.
- Golden audio regression fixtures are still needed for Consonant Tamer and changed-material calibration.
- The `.pkg` path still needs Developer ID signing, notarization, stapling, and clean-account install smoke.
