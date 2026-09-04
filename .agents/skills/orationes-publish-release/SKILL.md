---
name: orationes-publish-release
description: Run the complete approved Orationes release workflow when the user asks to bump the release and synchronize README, GitHub, the RePebble Developer Dashboard, the public Pebble App Store, and mobile My Apps. Do not use for local builds or partial publication.
---

# Publish Orationes everywhere

Turn an already approved Orationes development state into one traceable public release whose version, PBW, documentation, and listing description agree everywhere.

Use this skill for requests such as:

> Bump the release. Update README.md and the description in the Developer Dashboard, App Store, and My Apps.

That request explicitly authorizes the release commit, tag, GitHub Release, RePebble release, and listing-description update. It does not authorize unrelated source changes, screenshot replacement, or other listing changes.

Before acting, read and follow the complete `orationes-release`, `orationes-appstore`, and `orationes-build-audit` skills. Their safety and validation requirements still apply.

## Establish the release

1. Require a clean, pushed development state containing only changes the user has already approved. Inspect the current version, latest tag, commits since that tag, GitHub Releases, and the current RePebble release.
2. If the user did not name a version, choose the next semantic version from the shipped scope: patch for a narrowly compatible fix, minor for new user-facing behavior or content, and major only for an intentional breaking change. State the choice before mutation.
3. Derive release notes and README changes from the actual diff and source. Do not rely on an outdated README or invent features.
4. Update `package.json`, `package-lock.json` when present, and concise release references in `README.md`. Do not alter prayer text or runtime code merely for release preparation.

## Produce one candidate

1. Run `pebble clean` and `pebble build` after the version change.
2. Verify the PBW version, UUID, app type, and Emery-only compatibility. Record resource bytes, RAM footprint, free heap, PBW bytes, and SHA-256.
3. Install the exact PBW in Emery. Install it on the connected physical PT2 when the release workflow and prior approval call for it.
4. Run `git diff --check`; confirm `build/` is unstaged and canonical `content/*.txt` files remain ignored and untracked.

## Publish the same artifact

1. Commit only release metadata/documentation, push `main`, create an annotated version tag, and push it.
2. Create the GitHub Release from that tag and attach the tested `build/pebble-orationes.pbw`. Download the published asset to a temporary directory and verify its SHA-256 matches the local candidate.
3. Inspect `pebble publish --help`, then publish the same PBW and concise notes to the existing RePebble app ID `9882f741750c43eb8309777e`.
4. Preserve existing screenshots by choosing no screenshot upload unless the user explicitly asked to replace or add them.

## Synchronize the description

Keep the warm opening unless the user requests different copy:

> Your Pebble tells time. Orationes helps you make time for prayer.

Update the remaining description to reflect the current default menu, optional library, settings, accessibility, and offline behavior accurately. Keep it concise and useful rather than copying the README.

For the existing listing, do not use `pebble publish --description`; it only applies during new-app creation. Update the authenticated Developer Dashboard **Edit Listing** record or its official multipart PATCH flow described by `orationes-appstore`. Preserve title, source, visibility, category, companion fields, icons, banner, and screenshots.

## Verify all three RePebble surfaces

Do not treat a successful PATCH or release upload as final verification. Confirm all of the following show the new description and version:

1. Developer Dashboard app record and Emery asset record.
2. Public catalog API in both its general form and `?hardware=emery` form. The Emery form is the important proxy for mobile **My Apps**.
3. Canonical public page: `https://apps.repebble.com/9882f741750c43eb8309777e`.

RePebble propagation may be staggered. Poll read-only endpoints in short, bounded intervals while keeping the user informed. Do not say App Store or My Apps is updated until its corresponding surface has synchronized. Once the Emery API is current, a stale phone display is a client cache; recommend reopening My Apps, then removing and re-adding the installed listing only if needed.

## Final report

Report the version, commit and tag, build metrics, PBW size and digest, emulator/physical install results, GitHub Release and asset URL, Dashboard status, public App Store status, mobile Emery/My Apps status, preserved listing assets, README summary, and final clean Git status. Clearly identify any surface still awaiting external propagation.
