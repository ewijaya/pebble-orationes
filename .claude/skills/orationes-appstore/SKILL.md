---
name: orationes-appstore
description: Manages the existing Orationes Pebble App Store listing — release uploads, listing description, screenshots, and public verification — only when the user explicitly requests an App Store change.
disable-model-invocation: true
---

# Orationes Pebble App Store

The listing is public and shared with real users, so every change here is
outward-facing. Operate only on app ID `9882f741750c43eb8309777e`, change only
what the user asked for, and preserve every other field and prior release.

## Publishing a release

Versioned releases go through `scripts/release.py` (see the
`orationes-release` skill and `docs/releasing.md`). Do not run a top-level
`pebble publish` for a release: it rebuilds, so the uploaded PBW would differ
from the one that was tested and approved.

When publishing outside that workflow is explicitly requested:

- Read the installed `pebble publish --help` before composing a command.
- Verify PBW UUID, version, Emery-only platform, a clean build, and the
  physical approval the release requires.
- Release notes describe only shipped behavior.
- Native screenshots are raw Emery captures named like
  `emery_01_main-menu.png`, never framed mockups.
- `--replace-screenshots` deletes the prior set, so use it only with explicit
  authorization. Keep the main menu first when replacing the set.

## Editing the existing listing

`pebble publish --description` only applies when creating a new app; it does
not update this listing. Prefer the Developer Dashboard's **Edit Listing**
flow. If that UI is unavailable, use only the official endpoints the dashboard
itself uses:

1. Get the Firebase token via
   `pebble_tool.account.get_account(auth_provider="firebase")`. Keep it in
   memory only — never print, log, persist, or send it anywhere else.
2. Exchange it at `https://developer.repebble.com/api/auth/firebase/session`
   in an in-memory HTTP session.
3. GET `/api/dashboard/apps/9882f741750c43eb8309777e`, then PATCH that URL as
   multipart form data, re-sending existing values.
4. Preserve title, website, source, visibility, category, companion fields,
   icons, banners, and screenshots unless the user asked to change them.

Before any PATCH or upload, state what will change and confirm with the user
unless they already authorized that exact change.

## Verify

A successful response is not proof the storefront changed; it can lag the
dashboard. Check the dashboard record, the public catalog API (general and
`?hardware=emery`), and `https://apps.repebble.com/9882f741750c43eb8309777e`.
A cache-busting query can diagnose caching, but the canonical URL must
eventually show the change. Report any surface still pending.
