---
name: orationes-release
description: Prepare and publish an Orationes version when the user explicitly asks to bump, tag, release, or publish it. Do not use for ordinary builds or unapproved store publication.
---

# Orationes release

Produce one traceable release from the approved Orationes source and the exact PBW that was tested.

## Preflight

- Work from the repository root and inspect `git status`, `package.json`, recent commits, tags, configured remotes, and existing GitHub releases.
- Preserve unrelated user changes. Require clarity before releasing a dirty tree whose changes are not part of the release.
- Confirm the intended version and destinations: local commit, GitHub, Pebble App Store, or some combination. A request to build is not permission to publish.
- Verify `content/preces-latin.txt` and `content/litany-of-loreto.txt` remain ignored and untracked.

## Candidate

1. Run `pebble clean` and `pebble build`; fix errors only within the requested scope.
2. Record resource bytes, RAM footprint, free heap, and `build/pebble-orationes.pbw` size. The known RWX LOAD-segment warning is non-fatal unless its behavior changes.
3. Install and test on Emery. If the release changes interaction or appearance, install the same candidate with `pebble install --cloudpebble build/pebble-orationes.pbw` and stop for physical approval when requested.
4. Refresh screenshots before another `pebble clean`, because clean removes `build/emulator-screenshots/`.
5. Change only the intended version and release documentation. Rebuild after the version change so PBW metadata matches.

## Publication

- Commit only approved release files. Never commit `build/` or canonical files under `content/`.
- Follow the repository's existing annotated-tag convention. Push the release commit before its tag.
- Create the GitHub Release from the existing tag and attach `build/pebble-orationes.pbw`; verify the uploaded asset name and size.
- Publish to the Pebble App Store only when explicitly requested. Read the installed `pebble publish --help` first and use the Orationes App Store skill for listing details or screenshots.
- Verify the GitHub latest release, public App Store version, changelog, screenshots, and final clean Git status. Report any destination that was not updated.

