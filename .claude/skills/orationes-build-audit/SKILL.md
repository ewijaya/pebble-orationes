---
name: orationes-build-audit
description: Builds and statically audits Orationes for Emery — compiler output, memory budgets, PBW metadata, ignored files, and unexpected platform or network changes — without publishing. Use when build verification is the main task.
---

# Orationes build audit

Produce a reproducible build check. Nothing here tags, pushes, or publishes.

1. Record the working directory, `git status`, branch, `pebble --version`
   (tool and active SDK), and `package.json` target platforms.
2. Run `python3 scripts/test.py` for host checks (content hashes, generated
   catalog/resource freshness, C and JS tests).
3. Run `python3 scripts/check_release.py`. It does a clean build and checks
   Emery-only metadata, bundle size, resources, and RAM/heap budgets from
   `tests/build-budgets.json`. If you build by hand instead, use `pebble clean`
   then `pebble build`.
4. Record resource bytes, RAM footprint, and free heap from the build output.
   Measure PBW bytes and SHA-256 directly, and inspect its metadata when version
   or platform matters.
5. Confirm the bundle targets only `emery` and is an app, not a watchface. The
   PebbleKit JS Clay settings companion in `src/pkjs/` is expected. Flag any
   new network access beyond Clay's configuration page, because prayers are
   meant to work fully offline.
6. Run `git diff --check`, review changed paths, and confirm `build/`,
   `.release/`, and `content/*.txt` are ignored and unstaged.
7. Compare metrics with the nearest release (`docs/verification.md`, or
   `docs/release-status.json`). Explain meaningful growth; a small increase is
   not a failure by itself.

Fix build errors only when the user asked for fixes; otherwise diagnose and
report. The SDK linker warning about an RWX LOAD segment is established and
non-fatal: mention it, but do not call the build failed because of it.
