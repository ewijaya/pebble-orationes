# Store assets

Listing artwork for the Pebble App Store entry.

- Developer dashboard: <https://developer.repebble.com/dashboard>
- Public listing: <https://apps.repebble.com/orationes_9882f741750c43eb8309777e>

| File | Size | Used for |
| --- | --- | --- |
| `banner-720x320.png` | 720x320 | Listing header banner |
| `icon-144.png` | 144x144 | Large app icon |
| `icon-80.png` | 80x80 | Small app icon |
| `source/icon-master-1254.png` | 1254x1254 | Master artwork the icons are downscaled from |

The icons are square crops of the master mark; the banner adds the wordmark and
the "large print / high contrast / offline" tagline. Regenerate the icons from
`source/icon-master-1254.png` so every size stays visually identical.

These are listing assets only. The in-watch menu icon is a separate asset at
`resources/images/orationes-menu-icon.png`, declared as `MENU_ICON` in
`package.json`.
