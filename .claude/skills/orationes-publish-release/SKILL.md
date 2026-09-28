---
name: orationes-publish-release
description: End-to-end Orationes release — bump the version and sync README, GitHub, the RePebble Developer Dashboard, the public App Store, and mobile My Apps. Use only when the user asks to release everywhere.
disable-model-invocation: true
argument-hint: "[version]"
---

# Publish Orationes everywhere

Turn an already-approved development state into one traceable public release
whose version, PBW, documentation, and listing description agree everywhere.

A typical request:

> Bump the release. Update README.md and the description in the Developer
> Dashboard, App Store, and My Apps.

That authorizes the release commit, tag, GitHub Release, RePebble release, and
the listing description. It does not authorize unrelated source changes,
screenshot replacement, or other listing edits.

Before acting, read these files in full; their safety rules still apply:

- `.claude/skills/orationes-release/SKILL.md` — the `release.py` workflow
- `.claude/skills/orationes-appstore/SKILL.md` — listing rules
- `.claude/skills/orationes-build-audit/SKILL.md` — build checks
- `docs/releasing.md`

## 1. Establish the release

1. Require a clean, pushed tree with only already-approved changes. Check the
   current version, latest tag, commits since it, GitHub Releases, and the
   current RePebble release.
2. If no version was named, choose semver from the shipped scope — patch for a
   narrow compatible fix, minor for new user-facing behavior or content, major
   only for an intentional break — and tell the user before changing anything.
3. Derive notes and README changes from the actual diff since the last tag,
   not from an old README. Don't invent features.
4. Write `docs/releases/VERSION.md` and update
   `docs/releases/store-description.txt`, keeping the opening line unless the
   user asks otherwise:
   > Your Pebble tells time. Orationes helps you make time for prayer.

   Describe the current default menu, the library, settings, accessibility,
   and offline behavior accurately and concisely — not a copy of the README.
5. Bump `package.json` and both `package-lock.json` version fields; update
   `README.md`, `CHANGELOG.md`, and `docs/prayer-list.md` if relevant. Don't
   touch prayer text or runtime code for release prep. Commit these on `main`.

## 2. Prepare and approve one candidate

Run `release.py prepare` with `--destination github --destination appstore
--description docs/releases/store-description.txt --physical`, as in
`orationes-release`. Report the metrics, PBW size, and SHA-256, then stop for
the user's physical approval of this exact candidate. This is the one planned
pause; don't continue on installation alone.

## 3. Publish the same artifact

Run `release.py publish VERSION --approve-publish VERSION --approve-physical`.
It pushes, tags, creates the GitHub Release, uploads the frozen PBW to app
`9882f741750c43eb8309777e`, and PATCHes the description while preserving
title, source, visibility, category, companions, icons, banner, and
screenshots. Don't use `pebble publish --description` (new apps only) or a
top-level `pebble publish` (it rebuilds).

## 4. Verify all RePebble surfaces

A successful PATCH or upload is not verification. Confirm the new version and
description on:

1. The Developer Dashboard app record and its Emery asset record.
2. The public catalog API, both general and `?hardware=emery`. The Emery form
   is the closest proxy for mobile **My Apps**.
3. `https://apps.repebble.com/9882f741750c43eb8309777e`.

Propagation can be staggered. Poll read-only endpoints in short, bounded
intervals (`release.py verify VERSION --attempts 3`), giving the user a
one-line status each round, and keep going until the surfaces are current or
the bound is reached. Once the Emery API is current, a stale phone display is a
client cache: suggest reopening My Apps, then removing and re-adding the app
only if needed.

## Final report

Version, commit and tag, build metrics, PBW size and digest, emulator and
physical install results, GitHub Release and asset URL, Dashboard status,
public App Store status, Emery/My Apps status, preserved listing assets,
README summary, and final `git status`. Clearly name any surface still waiting
on propagation.
