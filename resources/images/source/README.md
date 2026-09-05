# Native Orationes mark

`orationes-menu-icon.svg` is the approved connected regular-cross design.
Its 25×25 PNG is the launcher asset. The header draws the same pixel mask in
its current foreground color, keeping the mark identical in light and dark mode.

Approved on 2026-09-05: the regular cross meets the circle at all four ends,
with a raised horizontal bar. Earlier proposals and previews were archived.
User-provided proportion reference:
https://commons.wikimedia.org/wiki/File:Opus_Dei_cross.svg

After an approved vector edit, regenerate both representations from the repo root:

```sh
rsvg-convert resources/images/source/orationes-menu-icon.svg -o resources/images/orationes-menu-icon.png
python3 scripts/generate_brand_icon.py
```

The mask generator requires Pillow as a development dependency only.
