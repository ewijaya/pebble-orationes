# Release verification

## v0.8.1 — 2026-09-05

Saving Slot 3 as Aspirations in the actual mobile configuration page produced
`TypeError: Illegal invocation` in the phone log. The timer passed directly to
the sync helper was invoked with the helper's options object as its receiver.
The mobile WebView rejected that call before the settings message was sent.
Wrapping both timer functions preserves their global receiver; parse errors
and queue errors now have separate log messages.

`tests/test_phone_config.js` exercises the actual companion entry point and
installed Clay parser with WebView-style timer receiver checks. It reproduced
the original exception before the fix and passes afterward, covering Slot 3
as Aspirations, integer/toggle conversion, replacement saves, stale ACKs,
pending-save recovery after restart, and confirmation cleanup.

`python3 scripts/check_release.py` passed all host/content checks and the clean
Emery build, metadata, and memory budgets. Resources remain 28,686 B, static
RAM 64,977 B, heap 66,095 B, and virtual image 64,980 B. Candidate PBW size is
776,595 B; the established SDK RWX warning remains. Watch C code is unchanged.

The corrected candidate was installed and launched on the physical PT2.
`build/pt2-clay-sync-fixed.png` confirms Aspirations in Slot 3 after the pending
mobile save was recovered. The user then tried further settings through MyApp
and confirmed that Save Settings works on the physical PT2. This approval
covers the fix before the version-only bump to 0.8.1.

The versioned 0.8.1 PBW passed the clean build, host/content checks, metadata,
and memory budgets, then installed successfully on Emery and the physical PT2.
All five reader screenshot comparisons passed using the QA Python environment
(the initial screenshot invocation used a Python without Pillow). Listing
artwork is unchanged because this release changes only phone synchronization.

Release PBW SHA-256:
`681bbbfdb9c840cf89333deeaa87b7d1cef7dcce8e02044c33da005f4a0e6207`.

## v0.8.0 — 2026-09-05

The final candidate was built with Pebble Tool 5.0.40 and SDK 4.33.1 for Emery
only. Its UUID remains `9f17d477-cef1-4512-8536-f01d50bb07a3`; the PBW metadata
reports `0.8.0`. The bundled Clay companion continues to provide phone settings.

| Check | Result |
| --- | --- |
| Clean build, PBW metadata, process header, and memory budgets | Passed |
| Sanitizer-backed C host tests and JS regression tests | Passed |
| Settings schema migration, interrupted writes, invalid messages, and acknowledgments | Passed |
| Generated catalog and complete C-literal/packaged Preces equality | Passed |
| Prayer content comparison against v0.7.0 | Ten prayer literal arrays identical; all six other guarded content files byte-identical |
| Existing prayer rendering | Five baseline comparisons passed outside the intentional progress-indicator and former shadow regions |
| Continue and reader lifecycle | Preces and Aspirations scroll, exit/relaunch, Resume, font-size round trip, and Start again passed |
| Library and pinning | Categories, prayer cards, pin confirmation, return selection, and empty-favorites access passed |
| Reader bounds | Preces in Large/Light and Litany in Extra Large/Dark reached stable bottoms, clamped at both ends, and returned to the exact top pixels |
| Appearance preflight on the same UI source | All six navigation palettes in both appearances, cancel/save, relaunch persistence, eight independent title/appearance combinations, font previews, and shortcut clearing passed |
| Phone preview logic | All 48 appearance/title/navigation combinations passed host checks |
| Physical PT2 | Exact final PBW installed successfully; a subsequent launch and screenshot confirmed the new header and existing shortcuts |
| Release screenshots | Eight native 200×228 captures refreshed and visually reviewed with their optional 600×800 frames; main menu retains Memorare |

The appearance preflight used the same UI source before the version-only bump.
The final v0.8.0 PBW passed the clean release gate, five reader comparisons, and
the complete `scripts/qa_flows.py` run. Local evidence is under
`build/regression-screenshots/`, `build/qa-flows/`, and
`build/pt2-v0.8.0-open.png`; generated evidence is ignored by Git.

| Build metric | v0.7.0 release | v0.8.0 release |
| --- | ---: | ---: |
| Resources | 24,969 B | 28,686 B |
| Static RAM | 65,393 B | 64,977 B |
| Initial available heap | 65,679 B | 66,095 B |
| PBW | 769,283 B | 775,762 B |

Loaded image: 63,812 B. Virtual image: 64,980 B, leaving 555 B within the
16-bit virtual-image limit. Preces allocates its 3,712-byte resource on first
use; custom fonts are shared and cached until app exit. The known SDK RWX
LOAD-segment warning is the only compiler/linker warning.

Final PBW SHA-256:
`47dc0aa94b30dd61e95d4f906e248b59258486dac9628439dd77fef44c10aadd`.

Hands-on physical touch/hold behavior, wrist-distance readability, the actual
phone configuration page, and reminder presentation still require device QA.
Reminder timing, weekday selection, and seasonal boundaries are covered by host
tests; this release did not repeat the earlier simulated-noon experiment below.
Prayer wording, mystery content, and weekday mappings are unchanged. No guided
Rosary was added. See [UI implementation notes](ui-refresh.md) for design details.

## v0.7.0 — 2026-09-05

Tested locally with Pebble Tool 5.0.40, SDK 4.33.1, and the Emery emulator on
an Apple Silicon Mac. This record covers the v0.7.0 release candidate.

| Check | Result |
| --- | --- |
| C host tests with AddressSanitizer and UndefinedBehaviorSanitizer | Passed |
| Legacy settings migration, interrupted writes, corrupt-record recovery | Passed |
| Phone message validation, failed saves, duplicate batches, acknowledgments | Passed |
| JS retries, stale acknowledgments, restart recovery, duplicate-slot swaps | Passed |
| Stable catalog IDs, generated output, and destination resolution | Passed |
| Prayer-source integrity | All seven guarded content files unchanged |
| Weekdays, Easter boundaries, DST scheduling, stale reminder limits | Passed |
| Clean Emery build and bundle metadata | Passed |
| Release presentation | Seven raw and framed screenshots refreshed and visually reviewed |
| Physical watch installation | Exact v0.7.0 PBW installed successfully through CloudPebble |
| Existing prayer rendering | Five screenshots match the baseline pixel for pixel |
| Preces and Aspirations | Scroll, double-Select exit, relaunch, Resume, font-size round trip, and Start again passed |
| All Prayers | Category wrapping, card browsing, pinning, return selection, and opening the pinned prayer verified |
| Empty shortcuts | Library remains accessible; pinning selects slot 1 and opens Preces correctly |
| Remember Place | Off/On watch UI verified; disabling persistence covered by host tests |

The screenshot baselines cover Preces and Angelus in Large/Light, Aspirations in
Large/Light and Extra Large/Dark, and a prayer card in Extra Large/Dark. The flow
checks allow up to three vertical pixels for native scroll-coordinate rounding.
Evidence is retained in `build/regression-screenshots/` and `build/qa-flows/`;
build outputs are intentionally ignored by Git.

The timer callback was observed at simulated local noon on September 5, 2026,
and it scheduled the following day's wakeup. A firmware missed-wakeup alert from
the clock experiments obscured the reminder UI, so reminder presentation and
Select-to-open were not verified in that experiment. Restarting the emulator
cleared the alert, after which the reader and library checks passed again.
The CLI clock-reset behavior is documented in the development guide.

| Build metric | Baseline | Final |
| --- | ---: | ---: |
| Resources | 24,969 B | 24,969 B |
| Static RAM footprint | 59,990 B | 65,393 B |
| Available heap reported by build | 71,082 B | 65,679 B |
| Release PBW | — | 769,283 B |

Release PBW SHA-256: `3d787d33f503c19206362bfc83efda61f7befc1c2b6b587acbe4826c5b075047`.

The PBW includes the existing Clay companion and its source map. The SDK still
emits its established non-fatal warning about an RWX LOAD segment. No other build
warnings were observed in the final clean build.

The exact release PBW installed successfully on the physical watch through
CloudPebble. This confirms installation; physical touch/hold behavior,
the configuration page in the actual phone app,
and reminder presentation still need device QA. Prayer wording, Rosary mystery
data, and the weekday mapping were preserved; no guided Rosary was added.

See [Developing Orationes](development.md) for commands, storage layout, and the
catalog workflow. GitHub Actions is configured for host checks; the Pebble build
and emulator checks run through the local release gate.
