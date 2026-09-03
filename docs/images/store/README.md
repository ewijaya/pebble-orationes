# Store assets

Listing artwork for the Pebble App Store entry.

- Developer dashboard: <https://developer.repebble.com/dashboard>
- Public listing: <https://apps.repebble.com/orationes_9882f741750c43eb8309777e>

| File | Size | Used for |
| --- | --- | --- |
| `banner-720x320.png` | 720x320 | Listing header banner |
| `icon-144.png` | 144x144 | Large app icon |
| `icon-80.png` | 80x80 | Small app icon |
| `social-preview-1280x640.png` | 1280x640 | GitHub repository social preview |
| `source/icon-master-1254.png` | 1254x1254 | Master artwork the icons are downscaled from |
| `source/social-preview-1280x640.svg` | vector | Editable source for the social preview |

The icons are square crops of the master mark; the banner adds the wordmark and
the "large print / high contrast / offline" tagline. Regenerate the icons from
`source/icon-master-1254.png` so every size stays visually identical.

GitHub's social preview is a fixed 2:1 crop, so `social-preview-1280x640.png`
is a native 2:1 redraw rather than a rescale of the 2.25:1 store banner, which
would lose 40px from each side. All of its content clears GitHub's recommended
40pt safe border. Edit `source/social-preview-1280x640.svg` and re-render with:

```sh
rsvg-convert -w 1280 -h 640 \
  docs/images/store/source/social-preview-1280x640.svg \
  -o docs/images/store/social-preview-1280x640.png
```

These are listing assets only. The in-watch menu icon is a separate asset at
`resources/images/orationes-menu-icon.png`, declared as `MENU_ICON` in
`package.json`.
