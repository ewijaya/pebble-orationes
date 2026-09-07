# My Apps artwork after a developer install

## Findings — 2026-09-07

The user's phone showed Orationes active and its updated Clay page working, but
without artwork in the mobile app. The general and `?hardware=emery` catalog
responses both returned the correct Orationes UUID, two icon URLs, and seven
screenshots. All nine image URLs returned HTTP 200 and valid PNGs, including the
`assets.repebble.com` URLs used by the mobile catalog. No artwork was replaced.

The official mobile app source explains a matching failure path:

- [PBW-to-locker conversion](https://github.com/coredevices/mobileapp/blob/d52101ad3d8940c5aa392d6f224e774cb6f5ce84/libpebble3/src/commonMain/kotlin/io/rebble/libpebblecommon/disk/pbw/PbwApp.kt#L58)
  marks the entry as sideloaded, sets store metadata to null, and does not populate
  screenshot/list image URLs.
- [Developer installation](https://github.com/coredevices/mobileapp/blob/d52101ad3d8940c5aa392d6f224e774cb6f5ce84/libpebble3/src/commonMain/kotlin/io/rebble/libpebblecommon/locker/Locker.kt#L254)
  inserts or replaces the local locker entry. Normal remote synchronization
  [does not overwrite sideloaded entries](https://github.com/coredevices/mobileapp/blob/d52101ad3d8940c5aa392d6f224e774cb6f5ce84/libpebble3/src/commonMain/kotlin/io/rebble/libpebblecommon/locker/Locker.kt#L218).
- [The mobile UI](https://github.com/coredevices/mobileapp/blob/d52101ad3d8940c5aa392d6f224e774cb6f5ce84/pebble/src/commonMain/kotlin/coredevices/pebble/ui/LockerUtil.kt#L486)
  reads those image URLs from the locker entry.

This is a source-backed diagnosis consistent with the observed developer-install
sequence, not an inspection of the user's private phone database or confirmation
of their exact mobile app revision. The PBW's watch menu icon is separate from
the mobile store artwork. Changing that icon would not populate these URLs.

## Recover without losing the Clay update

Keep the working developer installation until a store release containing the
Clay update has been published and verified. Before that, reinstalling from the
store restores the earlier companion without Continue First in Clay.

After v0.10.1 is verified live, open the Orationes listing from the Pebble mobile
app's store and confirm the version. If **Add To Watch** is offered, use that
store route to restore the store-linked entry, then reopen My Apps and Settings.
Do not remove the app merely to troubleshoot artwork: removal can clear watch
preferences and saved reading places. If the mobile UI does not offer the store
route, inspect its screenshot/options before deciding on removal and re-addition.

Confirm on the actual phone that the image is back and Clay still shows
**Orationes v0.10.1** and **Continue First**. API propagation alone does not prove
that a sideloaded local entry or the phone's cache has changed.

### Open the native store listing from a normal HTTPS link

Send [the phone helper page](https://ewijaya.github.io/pebble-orationes/) to the
phone, open it in Android Chrome, and tap **Open in Pebble**. The page uses an
explicit user-tapped Android intent, with a direct `pebble://` link as an
alternative. Pasting a custom scheme into Chrome or WhatsApp is not a reliable
way to launch it; [Chrome documents the user-gesture requirement](https://developer.chrome.com/docs/android/intents).

Both links specify the RePebble API as the `source`, so the mobile app opens
the intended store entry rather than defaulting to the older Rebble feed.
The [official deep-link handler](https://github.com/coredevices/mobileapp/blob/d52101ad3d8940c5aa392d6f224e774cb6f5ce84/pebble/src/commonMain/kotlin/coredevices/pebble/PebbleDeepLinkHandler.kt)
routes this to a store listing without the local locker UUID. This is a recovery
attempt, not proof that the phone's artwork has been restored. Confirm on the
actual phone before recording success.

The helper is a static, script-free page in `site/`. Its dedicated Pages workflow
publishes only that directory; it does not build, install, or release the PBW.
