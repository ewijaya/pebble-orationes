---
name: orationes-appstore
description: Manage the existing Orationes Pebble App Store release, listing metadata, screenshots, or public verification when the user explicitly requests an App Store change. Never publish or replace assets implicitly.
---

# Orationes Pebble App Store

Operate on app ID `9882f741750c43eb8309777e` and preserve unrelated listing fields and releases.

## Release publication

- Inspect `pebble publish --help` from the installed tool before composing a command.
- Verify the PBW UUID, version, Emery platform, clean build, and physical approval expected for the release.
- Use release notes that describe only shipped behavior. Use raw Emery captures whose basenames begin with `emery_`.
- `--replace-screenshots` removes the prior set and therefore requires explicit authorization. Keep the main menu first when replacing the showcase set.
- After publishing, verify the public version, PBW, changelog, screenshot count, and visibility.

## Existing listing metadata

`pebble publish --description` is for new-app creation and does not update an existing listing. Prefer the authenticated Developer Dashboard's **Edit Listing** flow.

If UI control is unavailable, use only the official RePebble endpoints already used by the dashboard:

1. Obtain the Firebase token through `pebble_tool.account.get_account(auth_provider="firebase")`; never print, persist, or transmit it elsewhere.
2. Exchange it at `https://developer.repebble.com/api/auth/firebase/session` in an in-memory HTTP session.
3. GET `/api/dashboard/apps/9882f741750c43eb8309777e`, then PATCH that same URL as multipart form data.
4. Preserve title, website, source, visibility, companion fields, icons, banners, and screenshots unless the user explicitly requested their change.

The public storefront can lag the dashboard. Verify the ordinary public URL after synchronization rather than declaring success from the update response alone. A cache-busting query may diagnose caching, but the canonical URL must eventually show the change.

