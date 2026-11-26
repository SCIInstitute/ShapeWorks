# ShapeWorks Release Process

This document outlines the steps to release a new version of ShapeWorks.


* Create a new branch for the release.  E.g. `git co -b release_v6.5`

`master` always keeps the `-dev` version string.  The release version number lives
only on the release branch, so there is no need to bump `master` back afterwards.

* Update the version number in `CMakeLists.txt` **on the release branch**

```
SET(SHAPEWORKS_MAJOR_VERSION 6 CACHE INTERNAL "Major version number" FORCE)
SET(SHAPEWORKS_MINOR_VERSION 5 CACHE INTERNAL "Minor version number" FORCE)
SET(SHAPEWORKS_PATCH_VERSION 0 CACHE INTERNAL "Patch version number" FORCE)
SET(SHAPEWORKS_VERSION_STRING "6.5.0-RC1")
SET(SHAPEWORKS_VERSION "${SHAPEWORKS_MAJOR_VERSION}.${SHAPEWORKS_MINOR_VERSION}.${SHAPEWORKS_PATCH_VERSION}")
```

* Confirm the API version is set correctly in the following files

The C++ side and the `swpython`/`swpip` wrappers derive it from `SHAPEWORKS_MAJOR/MINOR_VERSION`. CMake fails if the Python side doesn't match:

- `Python/shapeworks/shapeworks/api_version.py`

```
API_VERSION = "6.5"
```

- `install_shapeworks.sh` 

```
SW_MAJOR_VERSION=6.5
```

- `install_shapeworks.bat`

```
python -c "import sys; print('\n'.join(sys.path))" > "%USERPROFILE%\.shapeworks\python_path_6.5.txt"
python -c "import sys; print(sys.prefix)" > "%USERPROFILE%\.shapeworks\python_home_6.5.txt"
echo %PATH% > "%USERPROFILE%\.shapeworks\path_6.5.txt"
```

- `devenv.sh`

```
SW_MAJOR_VERSION=6.5
```

* Update Release Notes

- Update `docs/about/release-notes.md` with the new release notes

* Create Walkthrough Video

* Update Documentation
- Update release screenshot in `docs/index.md`
- Add tour video to `docs/index.md`

* When release candidate is final, set the version number in `CMakeLists.txt` to the final version number

* Tag and publish the release, in this order

1. Commit the release notes first. The Mac and Linux installers include
   `release-notes.md` and the built docs, so changing the notes means rebuilding them.
2. Tag the release on the release branch with an annotated tag,
   `git tag -a v6.5.0 -m "ShapeWorks 6.5.0"`, and push the tag. The installers are named
   from `git describe`, so a build of an untagged commit is named like
   `ShapeWorks-v6.5.0-14-g13800e7e7` instead of `ShapeWorks-v6.5.0`.
3. Wait for the builds the tag push starts, and take the installers from those runs.
   GitHub scopes caches to a ref, so the tag build can't use the release branch's caches.
   If the release changed `build_dependencies.sh` or `install_shapeworks.sh`, every
   platform rebuilds its dependencies, so these builds take much longer than usual.
4. Sign and notarize the Mac installers (Intel and Arm64). CI doesn't sign them.
5. Build the source archive with the LFS content. GitHub's own source archives only hold
   LFS pointer files. Run `Support/make_source_zip.sh v6.5.0 <output_dir>` to create
   `ShapeWorks-v6.5.0-src-with-git-lfs.zip`.
6. Create the GitHub release from the tag, and upload the installers and the source archive.
7. Point the documentation's `latest` version at the release (see below).
8. Update `version.json` on the web server (see below).

* Merge the release branch back to `master`.  Fixes and doc updates made during the
  release cycle live on the release branch and come back in this one merge; take
  `master`'s `SHAPEWORKS_VERSION_STRING` (the next `-dev` version) over the release
  branch's when resolving

* Bump `CMakeLists.txt` on `master` to the next `-dev` version **before** the next
  push to `master`

This is not cosmetic.  The docs version comes from `CMakeLists.txt`, so as long as `master` still
carries the version that was just released, every `master` build republishes its own docs on top of
the released version's — which is how the 6.7 docs ended up serving 6.8 release notes and
bundled-Python install instructions.  `deploy_docs.sh` now refuses to publish `master` over
whichever version holds `latest`, so getting this wrong fails the docs deploy instead of corrupting
a shipped version.

* Point the documentation's `latest` version at the new release

The docs site publishes one version per minor release, taken from `CMakeLists.txt`.  Builds of
`master` are always published as `<version> (dev)` under the `dev` alias, so bumping
`CMakeLists.txt` to the next development version automatically starts a new dev version in the
version dropdown.  The `latest` alias, which is what the site redirects to, has to be moved to the
released version by hand: run the **Mac Arm64 Build** workflow manually with the release tag
selected as the ref, `deploy_docs` checked, and `docs_alias` set to `latest`.  That rebuilds the
release's docs from its tag, moves `latest` onto it, titles it `<version> (latest)` in the version
dropdown, and drops the `(latest)` suffix from the version that previously held the alias.

Everything published for a version — the release notes and guides, but also the API reference and
`ShapeWorksCommands.md`, which are generated by running the binaries that were just built — comes
from the ref the workflow runs on.  Rebuild a version from its own tag or release branch, never
from `master`, or the version gets another release's API docs.

* Update `version.json` on the web server

Studio checks for updates against `https://www.sci.utah.edu/~shapeworks/version.json`
(`Studio/Interface/UpdateChecker.cpp`).  It reads the block for its own platform, compares
`major`/`minor`/`patch` against the version compiled into the running build, and if the file is
newer it shows a "New version available" dialog pointing at the GitHub releases page, followed by
`message`.

Until this file is updated **no existing installation is told the release exists**, so do it once
the binaries are actually downloadable.  The file lives in the `shapeworks-www` checkout:

```json
{
    "windows" : { "major": 6, "minor": 8, "patch": 0, "message": "6.8.0 has been released" },
    "linux"   : { "major": 6, "minor": 8, "patch": 0, "message": "6.8.0 has been released" },
    "macos"   : { "major": 6, "minor": 8, "patch": 0, "message": "6.8.0 has been released" }
}
```

The three platform keys must be exactly `windows`, `linux` and `macos` — they are matched against
`StudioUtils::get_platform_string()`.  A missing or misspelled key throws a JSON exception that is
swallowed unless the check was triggered manually, so a typo here fails silently.  Platforms can
hold different versions if a build is late, which is the reason for the per-platform blocks.

## Patch releases

A patch release (e.g. 6.5.1) carries selected fixes from `master` on top of the previous
release.

* Create the release branch from the previous tag, not from `master`: `git checkout -b release_v6.5.1 v6.5.0`

* Cherry-pick the fix PRs' merge commits, oldest first

```
git cherry-pick -x -m 1 <merge commit>
```

`-m 1` takes the PR's changes relative to `master` at the time, and `-x` records which commit
each pick came from.  A fix written on top of other work that isn't being backported will
conflict.  Adapt it to the release branch instead of pulling in more PRs.

* Set the patch version in `CMakeLists.txt`: `SHAPEWORKS_PATCH_VERSION` and
  `SHAPEWORKS_VERSION_STRING`

* Keep the API version at the minor version (`API_VERSION = "6.5"` in
  `Python/shapeworks/shapeworks/api_version.py`).  Users' installed Python packages are tied to it,
  so a patch release must not change it.  If a picked PR brings in `master`'s newer API version,
  set it back.

* Add a section for the patch release to `docs/about/release-notes.md`

* Build and run the tests, then push the branch and check that CI passes on every platform

* Tag and publish the release, following the steps above

* Merge the release branch back to `master` with a merge commit, not a squash.  The fixes are
  already on `master`, so take `master`'s side of every conflict except the release notes.  A
  squash leaves the release unmerged as far as git is concerned, and the next merge-back hits the
  same conflicts again.


