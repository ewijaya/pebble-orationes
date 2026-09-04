---
name: orationes-build-audit
description: Build and statically audit Orationes for Emery, reporting compiler results, memory metrics, PBW metadata, ignored files, and accidental platform or network changes. Use when build verification is the primary task.
---

# Orationes build audit

Run a reproducible compiler and packaging check without publishing anything.

1. Confirm the active directory, Git status, `pebble-tool` version, active SDK, and `package.json` target platform.
2. Inspect installed CLI help when command syntax is uncertain.
3. Run `pebble clean` followed by `pebble build`. Fix compiler/build errors only when the user asked for implementation or fixes; otherwise diagnose and report.
4. Record the build's resource size, RAM footprint, and free heap. Measure PBW bytes directly and inspect its metadata when version or platform correctness matters.
5. Confirm the bundle targets only `emery`, is an app rather than a watchface, and contains no PebbleKit JS or network code.
6. Run `git diff --check`, inspect changed paths, and verify `build/` plus canonical `content/*.txt` files are ignored and unstaged.
7. Compare metrics with the nearest relevant release when available. Explain meaningful growth; do not treat a small increase as failure by itself.

The linker warning about an RWX LOAD segment is an established non-fatal SDK/toolchain warning. Surface it, but do not report failure when the build otherwise succeeds.

