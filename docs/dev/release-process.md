# ShapeWorks Release Process

This document outlines the steps to release a new version of ShapeWorks.


* Create a new branch for the release.  E.g. `git co -b release_v6.5`

* Update the version number in `CMakeLists.txt`

```
SET(SHAPEWORKS_MAJOR_VERSION 6 CACHE INTERNAL "Major version number" FORCE)
SET(SHAPEWORKS_MINOR_VERSION 5 CACHE INTERNAL "Minor version number" FORCE)
SET(SHAPEWORKS_PATCH_VERSION 0 CACHE INTERNAL "Patch version number" FORCE)
SET(SHAPEWORKS_VERSION_STRING "6.5.0-RC1")
SET(SHAPEWORKS_VERSION "${SHAPEWORKS_MAJOR_VERSION}.${SHAPEWORKS_MINOR_VERSION}.${SHAPEWORKS_PATCH_VERSION}")
```

* Confirm the API version is set correctly in the following files

TODO - This should be automated from a single place

- `Studio/Python/PythonWorker.h`

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

* Update Release Notes

- Update `docs/about/release_notes.md` with the new release notes

* Create Walkthrough Video

* Update Documentation
- Update release screenshot in `docs/index.md`
- Add tour video to `docs/index.md`

* When release candidate is final, set the version number in `CMakeLists.txt` to the final version number

* After the release, set the version to the next development version


