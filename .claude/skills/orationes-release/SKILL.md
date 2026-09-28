---
name: orationes-release
description: Prepares, publishes, and verifies one tested Orationes version with scripts/release.py when the user explicitly asks to bump, tag, release, or publish. Not for ordinary builds.
disable-model-invocation: true
argument-hint: "[version] [github|appstore]"
---

# Orationes release

Releases use `scripts/release.py`: freeze one tested candidate, publish that
exact PBW, then verify public availability and sync the docs. Read
`docs/releasing.md` before running anything; it holds the maintained commands
and recovery steps.

Loading this skill to explain or maintain the workflow is not a release
request. Tags, GitHub Releases, and store uploads are public and hard to undo,
so act only within the destinations and version the user approved.

## Preflight

- Work from the repo root. Check `git status`, branch, remotes, recent
  commits and tags, versions in `package.json` and `package-lock.json`, and
  existing releases at each intended destination.
- Confirm version and scope: local only, GitHub, App Store, or both. A build,
  version bump, or GitHub release does not authorize store publication.
- Preparation needs a clean, committed tree. Do not satisfy that by committing
  unrelated work, stashing without approval, or discarding changes — ask.
- Confirm `build/`, `.release/`, `content/preces-latin.txt`, and
  `content/litany-of-loreto.txt` remain ignored and untracked.
- For a local-only request, make only the approved version/doc edits and build
  checks. `release.py prepare` needs a real destination; do not invent one.

## Prepare the candidate

1. Set the approved version in `package.json` and both `package-lock.json`
   version fields before the final build. Review the README, `CHANGELOG.md`,
   `docs/prayer-list.md`, release notes in `docs/releases/VERSION.md`, and (for
   the store) `docs/releases/store-description.txt`. Describe only shipped
   behavior; don't claim a destination is live yet. Commit only these files on
   `main`.
2. Make and review any intended screenshot changes before freezing (see
   `orationes-screenshots`). `pebble clean` deletes `build/`, so preserve raw
   captures first. The release helper keeps existing store screenshots.
3. Run, with the approved values:
   ```sh
   python3 scripts/release.py prepare VERSION --notes docs/releases/VERSION.md \
     --destination github --destination appstore \
     --description docs/releases/store-description.txt --physical
   ```
   This runs the clean build, host/content checks, screenshot comparisons, and
   emulator suites, then installs the same PBW on the connected PT2. It does
   not push, tag, or publish.
4. Inspect `.release/VERSION/manifest.json`, the frozen PBW, metrics, and QA
   evidence. Report resource bytes, RAM, free heap, PBW size, and SHA-256.
   The RWX linker warning is established and non-fatal.
5. Stop and ask the user to test this exact candidate on the watch. Installation
   is not approval of readability, touch, holds, or navigation. If the watch is
   not connected, stop before freezing — do not bypass the guard.

After freezing, do not rebuild or change the version and upload the result.
Changed source or PBW means a new candidate that needs its own review.
Candidate directories cannot be overwritten; if one exists, ask for a recovery
decision rather than deleting evidence or editing the manifest.

## Publish the approved candidate

Only after the user approves this candidate and its recorded destinations:

```sh
python3 scripts/release.py publish VERSION --approve-publish VERSION --approve-physical
```

The flags record approval the user actually gave; they never stand in for it.
The helper checks the commit and PBW hash are unchanged, pushes the commit
before its annotated tag, publishes GitHub before the store, uploads
`.release/VERSION/pebble-orationes.pbw`, syncs the reviewed description, and
preserves unrelated listing metadata. Do not substitute the mutable `build/`
PBW or run a top-level `pebble publish` (it rebuilds). For store specifics,
read `.claude/skills/orationes-appstore/SKILL.md`. Never expose auth tokens.

## Verify, sync, and recover

- Upload success is not completion. The helper hashes downloaded assets,
  checks GitHub latest, the dashboard, and the canonical public listing. Look
  at public screenshots yourself when the store is in scope. Don't claim the
  mobile My Apps cache is fresh without observing it.
- Only after every selected destination verifies does `publish` update the
  README availability line, GitHub notes, and `docs/release-status.json`, then
  commit and push them.
- If interrupted or propagation lags, run
  `python3 scripts/release.py verify VERSION --attempts 3` (read-only, safe to
  retry). Once propagation completes, rerun the approved `publish` command to
  finish doc sync. Mismatched assets, drafts, changed source, or altered
  listing fields need the user's review, not an automatic overwrite.

## Final report

Version, frozen PBW hash, test and physical evidence, per-destination
verification, documentation status, and final `git status`. Name any
destination that is not updated or still pending.
