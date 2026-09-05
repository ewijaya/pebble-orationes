# Image cleanup audit

Audited and cleaned up 2026-09-05. The sections below “Original audit” record
the state and recommendations before cleanup.

## Cleanup completed

Follow-up decision on 2026-09-05: the realistic 720×320 banner is now the
definitive social preview. Archived the previous `social-preview-1280x640.png`,
the entire `docs/images/store/source/` folder, and `scripts/render_promo.py`.
The store icon PNGs and both approved realistic banner PNGs remain unchanged.
This follow-up is recoverable from
`/Users/e_wijaya_ap/Desktop/orationes-social-preview-archive-2026-09-05-KGcYus/`,
with original paths and a SHA-256 manifest. The initial cleanup results below
describe the state before this follow-up.

- Image count: **76 → 26**. Image bytes: **15,077,133 → 2,132,421**
  (14.38 → 2.03 MiB), a reduction of 12.35 MiB.
- Removed `docs/design/`, retired campaign artwork, old banners, unused frames,
  redundant screenshots, and historical physical photos from the project.
- Preserved approved-icon provenance in `resources/images/source/README.md`.
- Relocated the two retained listing-icon SVGs into `docs/images/store/source/`.
  The social-preview SVG already embeds its screenshot and needs no separate raw input.
- Updated duplicate image references and store/campaign documentation.
- Simplified `scripts/render_promo.py` to export only retained SVG artwork.
  It no longer recreates old campaign assets or overwrites the banner.
- Optional frames now go to ignored `build/framed-screenshots/`; the screenshot
  workflow and README instructions reflect this.

Recovery archive on the original workstation:
`/Users/e_wijaya_ap/Desktop/orationes-image-archive-2026-09-05-KLDJUL/`.
`before-cleanup/` preserves the original project-relative paths, including
modified and untracked assets. `manifest.json` records backup sizes and SHA-256
hashes; `cleanup-actions.json` identifies archived files and relocated sources.
Every removed file was compared with its backup before removal. Restore selected
files from those paths if needed; the archive also contains pre-cleanup copies of
the edited documentation and scripts.

Validation: all retained pre-existing image bytes unchanged; the three retained
SVG exports match their PNGs pixel for pixel; all eight optional frames preserve
the native screenshot pixels at 2× scale; all six Markdown/HTML image references
resolve; the README contains only the approved banner. `git diff --check` passed.
No watch installation, release, or live store update was performed.

## Original audit

The project contains **76 image files totaling 15,077,133 bytes (14.38 MiB)**
under `docs/`, `resources/`, and `tests/`. This includes tracked files and
untracked work. Ignored build output, dependencies, and Git history are outside
the cleanup scope.

The audit checked repository references, image dimensions, generation scripts,
Git status, and SHA-256 duplicates. A file absent from the README is not
automatically unused: some are runtime resources, test baselines, store upload
assets, or source artwork. Live store and GitHub settings were not inspected.

## Keep

| Assets | Reason |
| --- | --- |
| `docs/images/store/orationes-banner-realistic-main-menu-master.png` | Approved README banner, 1881×836. |
| `docs/images/store/orationes-banner-realistic-main-menu-720x320.png` | Approved store-size export. Its different resolution serves a separate purpose. |
| `resources/images/orationes-menu-icon.png` and `resources/images/source/orationes-menu-icon.svg` | Active launcher asset and canonical editable source; also used to generate the native header mark. |
| All five `tests/screenshots/` images | Prayer rendering regression baselines. |
| All eight `docs/images/raw/` images | Current native screenshots and launcher reference, maintained for store and documentation use. |
| `docs/images/ui-refresh/categories.png`, `font-preview.png`, and `pin-confirmation.png` | Unique evidence embedded in `docs/ui-refresh.md`. |
| `docs/images/store/icon-80.png` and `icon-144.png` | Dedicated listing icons; independent of README artwork. |
| `docs/images/store/social-preview-1280x640.png` and its source SVG | Dedicated social-preview format; removing it is a separate branding decision. |

## Exact duplicates to consolidate

Keep the first path in each row. Update any references before removing the second.

| Canonical file | Redundant copy |
| --- | --- |
| `docs/images/store/orationes-banner-realistic-main-menu-master.png` | `docs/images/store/source/banner-realistic-main-menu-master.png` |
| `resources/images/orationes-menu-icon.png` | `docs/design/icon-proposals/centered-cross-connected-25.png` |
| `resources/images/source/orationes-menu-icon.svg` | `docs/design/icon-proposals/centered-cross-connected.svg` |
| `docs/images/store/source/social-preview-1280x640.svg` | `docs/images/promo/source/social-preview-1280x640.svg` |
| `docs/images/raw/continue.png` | `docs/images/ui-refresh/continue.png` |
| `docs/images/raw/navigation-colors.png` | `docs/images/ui-refresh/navigation-violet.png` |

`docs/images/store/banner-720x320.png` also exactly matches
`docs/images/store/source/banner-typographic-720x320.png`. Both are the older
typographic design, not the newly selected realistic banner. Their generator
must be corrected before consolidating them.

## Obsolete or optional assets

| Group | Count / size | Recommendation |
| --- | --- | --- |
| `docs/images/promo/` images | 22 / 284 KiB | Retire the former README hero, four gallery cards, category guide, and associated campaign files. Preserve or relocate the icon SVG sources and any input still required for the retained social preview before removing this directory's images. |
| `docs/images/framed/` | 8 / 42 KiB | Optional removal: no current document embeds these images. They are reproducible from the retained raw screenshots, but README instructions and the screenshot workflow still describe them. |
| `docs/design/icon-proposals/` | 9 / 7 KiB | Archive rejected bold/separated designs and review previews; consolidate the two approved source duplicates above. Preserve approval and proportion-reference notes alongside the canonical icon source. |
| Old typographic banner master, export, and generic banner | 3 / 2.21 MiB | Superseded locally by the approved realistic banner. Retire after correcting the banner generator and store guide. |
| `docs/images/store/source/banner-realistic-preces-backup-720x320.png` | 1 / 265 KiB | Superseded Preces-screen variant; archive with the old campaign artwork. |
| `docs/images/physical/` | 5 / 7.05 MiB | No current repository references. Archive original photos outside the project rather than discard them; these are original physical-device evidence, not reproducible screenshots. |
| `docs/images/store/source/icon-master-1254.png` | 1 / 873 KiB | Historical original referenced by the store guide. Archive if retiring historical branding sources. |

Counts in this section overlap the duplicate table and must not be added together.

## Required companion changes

1. `scripts/render_promo.py` currently regenerates the old campaign, writes the
   duplicate social-preview SVG, and copies the typographic artwork over
   `docs/images/store/banner-720x320.png`. Adjust or retire the affected generation
   steps so cleanup does not recreate obsolete images or restore the old banner.
2. Correct `docs/images/store/README.md`: it still calls the typographic design
   approved and says the native icon is unchanged.
3. Retire or revise `docs/images/promo/README.md`: its README-image and native-UI
   descriptions are stale. Preserve useful publication history and the store
   description text; they are not image-cleanup targets.
4. Point the two duplicate image references in `docs/ui-refresh.md` to the raw
   screenshots before removing the copies.
5. If dropping framed exports, update the documentation that promises checked-in
   frames while keeping the framing tool available for on-demand exports.

## Recommended cleanup order

First consolidate exact duplicates and repair their references. Then retire the
superseded campaign together with its stale generation paths. Finally move
original photos and design history into a dated archive outside the repository.
Keep a path manifest for recovery, especially for untracked or modified images
that cannot be recovered from the current Git commit.

Validate retained local image references and the README's single approved banner
after cleanup. No app rebuild, watch installation, or store publication is needed
for this file cleanup.
