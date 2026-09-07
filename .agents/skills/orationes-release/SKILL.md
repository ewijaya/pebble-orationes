---
name: orationes-release
description: Prepare and publish an Orationes version when the user explicitly asks to bump, tag, release, or publish it. Do not use for ordinary builds or unapproved store publication.
---

# Orationes release

Use the repository's `scripts/release.py` workflow: prepare one tested candidate,
publish that exact PBW, then verify public availability and synchronize release
documentation. Read [docs/releasing.md](../../../docs/releasing.md) before running
the workflow; it contains the maintained commands and recovery procedure.

Invoking this skill for explanation or maintenance is not a release request.

## Preflight

- Work from the repository root. Inspect Git status, branch, remotes, recent
  commits/tags, package and lockfile versions, and existing releases for the
  intended destinations. Use the Pebble Tool Python environment, with Pillow for QA.
- Confirm the version and scope: local preparation/commit, GitHub, App Store,
  or both. A build, version bump, or GitHub release does not authorize store
  publication. Select only approved destinations.
- Preserve unrelated changes. Preparation and publication require a clean,
  reviewed, committed tree; do not satisfy that gate by committing unrelated
  work, stashing it without approval, or discarding it.
- Verify `build/`, `.release/`, `content/preces-latin.txt`, and
  `content/litany-of-loreto.txt` remain ignored and untracked.
- For a local-only request, perform only the approved version/documentation
  edits, build checks, and any approved commit. `release.py prepare` requires a
  future publication destination; do not invent one to handle a local build.

## Prepare the candidate

1. Update the approved version in `package.json` and both lockfile version
   fields **before** the final build. Review README behavior, release notes,
   `docs/prayer-list.md` where applicable, and the complete store description
   when the store is selected. Remove stale development/version claims without
   claiming a destination is already live. Commit only approved release files
   on `main`, including reviewed notes and description files.
2. Refresh and review any intended screenshot changes before freezing the
   source. Preserve raw captures before `pebble clean`, which removes `build/`.
   The release helper preserves existing store screenshots; it does not upload
   replacements. Use the Orationes screenshots/App Store skills when those
   separate changes are actually requested.
3. Run `python3 scripts/release.py prepare VERSION --notes NOTES --destination
   DESTINATION --physical`, substituting the approved values. Repeat
   `--destination` for both `github` and `appstore`; add `--description FILE`
   for the store. This runs the clean build, host/content checks, screenshot
   comparisons, and full reader emulator suite, then installs the same PBW on
   the connected PT2. It does not push, tag, or publish.
4. Inspect `.release/VERSION/manifest.json`, its frozen PBW, metrics, and QA
   evidence. The manifest records the source commit, version, destinations,
   notes, description, PBW bytes, and SHA-256. Report resource bytes, RAM,
   free heap, and PBW size. The established RWX linker warning is non-fatal.
5. Obtain the user's physical approval of this exact candidate before
   publication; installation alone is not approval of readability, touch,
   holds, or navigation. The current helper requires both a physical install
   during preparation and explicit physical approval for publication. If the
   connection is unavailable, stop before freezing a publishable candidate;
   do not bypass its guards or fabricate approval.

Never rebuild or change versions after freezing and then upload the replacement.
A changed source or PBW requires a newly reviewed candidate. Existing candidate
directories cannot be overwritten; stop for a recovery decision rather than
deleting evidence or editing the manifest to defeat a guard.

## Publish the approved candidate

- Only with approval for the candidate and its recorded destinations, run
  `python3 scripts/release.py publish VERSION --approve-publish VERSION
  --approve-physical`. Approval flags express real user approval; they do not
  grant it. Publication also commits/pushes verification documentation to
  `origin/main`; ensure that synchronization is within the approved scope.
- Let the helper enforce the unchanged commit/PBW, push the release commit
  before its annotated tag, and upload `.release/VERSION/pebble-orationes.pbw`.
  When both destinations are selected, GitHub precedes the store. Do not
  substitute the mutable `build/` artifact or independently recreate releases.
- For store publication, read the installed `pebble publish --help` and use
  the Orationes App Store skill for listing-specific checks. Do not replace
  the helper with a top-level `pebble publish` invocation: that command rebuilds.
  The helper uses the installed publisher's upload path for the frozen PBW,
  synchronizes the reviewed description, and preserves unrelated listing
  metadata and artwork. Never expose authentication tokens.

## Verify, synchronize, and recover

- Upload success is not completion. The helper downloads and hashes selected
  destination assets, checks GitHub latest, and checks store dashboard
  version/notes/description plus the canonical public listing and changelog.
  Separately inspect public screenshots when the store is in scope. Do not
  claim mobile-client/My Apps cache freshness without observing it.
- Only after all selected destinations verify does the publish workflow update
  the README availability line, GitHub notes when selected, and
  `docs/release-status.json`, then commit/push the verification documentation.
  Prayer-directory content and feature descriptions require the earlier human
  review; this final step does not generate them automatically.
- If interrupted or propagation is delayed, inspect the local manifest and
  run `python3 scripts/release.py verify VERSION --attempts 3`. This performs
  public read-only checks and updates the local journal, with bounded retries;
  it does not upload or finish documentation synchronization. Report pending
  destinations if verification still fails instead of rebuilding/reuploading.
- Once propagation completes, rerun the approved `publish` command if needed
  to finish documentation synchronization. Existing mismatched assets, drafts,
  changed source, failed guards, or altered unrelated listing fields require
  review, not automatic overwrites or destructive repair.
- Finish with the version, frozen PBW hash, test/physical evidence, per-destination
  verification, documentation status, and final Git status. Explicitly identify
  any destination not updated or still pending.
