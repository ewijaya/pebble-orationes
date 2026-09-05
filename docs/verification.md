# v0.7.0 verification — 2026-09-05

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
