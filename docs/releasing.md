# One-candidate release workflow

`scripts/release.py` freezes a tested PBW, publishes that exact file only to
explicitly selected destinations, and records public verification. It does not
rebuild while publishing. It never replaces screenshots, deletes releases, or
prints authentication tokens.

## Prepare the reviewed development state

1. Finish implementation and review the actual diff. Choose a new semantic
   version; update `package.json`, both version fields in `package-lock.json`,
   the README, changelog, and prayer directory where applicable. Commit this
   approved state on `main`. Keep canonical `content/` files untracked.
2. Write reviewed release notes and, for the store, the complete desired listing
   description. Describe shipped behavior only; do not copy claims from an old
   release. Keep these files in `docs/releases/` and commit them too.
3. Run with the Pebble Tool Python environment (requires Pillow for emulator QA):

```sh
python3 scripts/release.py prepare 0.10.0 \
  --notes docs/releases/0.10.0.md \
  --description docs/releases/store-description.txt \
  --destination github --destination appstore --physical
```

The version above is an example, not a release authorization. Preparation requires
a clean tree, runs a clean build, host tests, screenshot comparisons, Help topics, phone/watch sync, menu/header regressions and the full
reader emulator suite, the automatic-resume matrix, and the single-reset/version
footer checks, then installs the exact PBW on the connected PT2. It copies
the PBW, evidence, metrics, source commit, notes, description, and digest into the
ignored `.release/0.10.0/` directory. Existing candidates cannot be overwritten.
No tag, push, release, or listing update occurs during preparation.

## Review and explicitly publish

Review the screenshots and test touch, holds, readability, and navigation on the
physical watch. When the user approves this exact candidate and its destinations:

```sh
python3 scripts/release.py publish 0.10.0 \
  --approve-publish 0.10.0 --approve-physical
```

The tool checks that the clean source commit and PBW hash still match, pushes the
commit before its annotated tag, creates the GitHub Release, and uploads the
frozen PBW to the existing Orationes store app. The installed publisher's upload
method is used because the top-level `pebble publish` command rebuilds first.
The dashboard PATCH preserves unrelated metadata, visibility, icons, banners,
companions, and screenshots. Existing version/asset mismatches stop the workflow;
they are never silently overwritten.

Verification downloads and hashes both assets, checks GitHub latest, dashboard
version/notes/description, and the canonical public listing and changelog.
Only after all selected destinations verify does it synchronize the README's
availability line, GitHub notes, and `docs/release-status.json`, committing and
pushing those documentation files. Other destinations are not advertised as
updated. Mobile-client caches still need separate hands-on verification.

## Interrupted or delayed publication

Publication is not atomic across services. `.release/<version>/manifest.json`
records partial progress; never equate upload success with public propagation.
Read-only verification can be retried without rebuilding or uploading:

```sh
python3 scripts/release.py verify 0.10.0 --attempts 3
```

Each attempt reports its result, with 20 seconds between attempts. If propagation
finishes after a stopped publish, rerun the approved `publish` command to verify
the existing matching assets and finish documentation synchronization. A failed
or unverified destination is reported as pending, not as a completed release.

The README availability line and `docs/release-status.json` record verified
publication; a prepared candidate is not a published release. The workflow's
guards, artifact checks, metadata preservation, and partial-status handling
have offline tests in `tests/test_release.py`.
