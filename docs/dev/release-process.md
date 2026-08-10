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

TODO - This should be automated from a single place

- `Libs/Application/Job/PythonWorker.h`

```
  constexpr static const char* python_api_version = "6.5";
```

- `Python/shapeworks/shapeworks/utils.py`

```
def get_api_version():
    return "6.5"
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

* Tag the release from the release branch

* Merge the release branch back to `master`.  Fixes and doc updates made during the
  release cycle live on the release branch and come back in this one merge; take
  `master`'s `SHAPEWORKS_VERSION_STRING` (the next `-dev` version) over the release
  branch's when resolving

* Point the documentation's `latest` version at the new release

The docs site publishes one version per minor release, taken from `CMakeLists.txt`.  Builds of
`master` are always published as `<version> (dev)` under the `dev` alias, so bumping
`CMakeLists.txt` to the next development version automatically starts a new dev version in the
version dropdown.  The `latest` alias, which is what the site redirects to, has to be moved to the
released version by hand: run the **Mac Arm64 Build** workflow manually with the release tag
selected as the ref, `deploy_docs` checked, and `docs_alias` set to `latest`.  That rebuilds the
release's docs from its tag and moves `latest` onto it.


