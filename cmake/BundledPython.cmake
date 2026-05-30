# BundledPython.cmake — Download a relocatable Python from python-build-standalone
#
# When USE_BUNDLED_PYTHON is ON, this module downloads a prebuilt, relocatable
# CPython into ${CMAKE_BINARY_DIR}/_bundled_python/python/ at configure time.
# The bundled Python is used ONLY at runtime/packaging — the build still uses
# conda's Python (found by find_package(Python3 ...)) for compilation.

option(USE_BUNDLED_PYTHON "Bundle standalone Python in distribution" OFF)

if(NOT USE_BUNDLED_PYTHON)
  return()
endif()

message(STATUS "USE_BUNDLED_PYTHON is ON — downloading standalone Python")

# ---------------------------------------------------------------------------
# Determine platform tag for python-build-standalone
# ---------------------------------------------------------------------------
set(BUNDLED_PYTHON_VERSION "3.12.8")
set(BUNDLED_PYTHON_RELEASE "20250106")

if(APPLE)
  execute_process(COMMAND uname -m OUTPUT_VARIABLE _arch OUTPUT_STRIP_TRAILING_WHITESPACE)
  if(_arch STREQUAL "arm64")
    set(_platform_tag "aarch64-apple-darwin")
  else()
    set(_platform_tag "x86_64-apple-darwin")
  endif()
elseif(UNIX)
  set(_platform_tag "x86_64-unknown-linux-gnu")
elseif(WIN32)
  set(_platform_tag "x86_64-pc-windows-msvc")
else()
  message(FATAL_ERROR "USE_BUNDLED_PYTHON: unsupported platform")
endif()

set(_archive_name "cpython-${BUNDLED_PYTHON_VERSION}+${BUNDLED_PYTHON_RELEASE}-${_platform_tag}-install_only_stripped.tar.gz")
set(_download_url "https://github.com/astral-sh/python-build-standalone/releases/download/${BUNDLED_PYTHON_RELEASE}/${_archive_name}")

set(_bundled_dir "${CMAKE_BINARY_DIR}/_bundled_python")
set(_archive_path "${_bundled_dir}/${_archive_name}")
set(BUNDLED_PYTHON_ROOT "${_bundled_dir}/python" CACHE PATH "Root of bundled standalone Python" FORCE)

if(WIN32)
  set(BUNDLED_PYTHON_EXECUTABLE "${BUNDLED_PYTHON_ROOT}/python.exe" CACHE FILEPATH "Bundled Python executable" FORCE)
else()
  set(BUNDLED_PYTHON_EXECUTABLE "${BUNDLED_PYTHON_ROOT}/bin/python3" CACHE FILEPATH "Bundled Python executable" FORCE)
endif()

# ---------------------------------------------------------------------------
# Download + extract (only if not already present)
# ---------------------------------------------------------------------------
if(NOT EXISTS "${BUNDLED_PYTHON_EXECUTABLE}")
  message(STATUS "Downloading bundled Python from ${_download_url}")

  file(MAKE_DIRECTORY "${_bundled_dir}")

  file(DOWNLOAD
    "${_download_url}"
    "${_archive_path}"
    SHOW_PROGRESS
    STATUS _download_status
  )
  list(GET _download_status 0 _download_code)
  if(NOT _download_code EQUAL 0)
    list(GET _download_status 1 _download_msg)
    message(FATAL_ERROR "Failed to download bundled Python: ${_download_msg}")
  endif()

  message(STATUS "Extracting bundled Python to ${_bundled_dir}")
  execute_process(
    COMMAND ${CMAKE_COMMAND} -E tar xzf "${_archive_path}"
    WORKING_DIRECTORY "${_bundled_dir}"
    RESULT_VARIABLE _extract_result
  )
  if(NOT _extract_result EQUAL 0)
    message(FATAL_ERROR "Failed to extract bundled Python archive")
  endif()

  # Clean up the archive to save space
  file(REMOVE "${_archive_path}")
else()
  message(STATUS "Bundled Python already present at ${BUNDLED_PYTHON_ROOT}")
endif()

# Verify extraction
if(NOT EXISTS "${BUNDLED_PYTHON_EXECUTABLE}")
  message(FATAL_ERROR "Bundled Python extraction failed — ${BUNDLED_PYTHON_EXECUTABLE} not found")
endif()

# ---------------------------------------------------------------------------
# Copy conda's extension modules into the bundled tree (Unix only).
# The build-time embedded interpreter uses conda's libpython, which expects
# C extension modules (math, _json, etc.) as separate .so files.
# python-build-standalone compiles these statically into its libpython,
# leaving lib-dynload nearly empty. Copying conda's extensions ensures
# the interpreter works in both the build tree and the installed app.
#
# On Windows this is NOT needed — the bundled Python's install_only variant
# already ships all .pyd files in DLLs/, and conda's .pyd files are
# ABI-incompatible (different libffi naming, different build).
# ---------------------------------------------------------------------------
find_package(Python3 3.12 REQUIRED COMPONENTS Interpreter)
get_filename_component(_conda_prefix "${Python3_EXECUTABLE}" DIRECTORY)  # .../bin or .../
if(WIN32)
  # On Windows, Python3_EXECUTABLE is directly in the conda prefix (no bin/ subdir)
  set(_conda_prefix "${_conda_prefix}")
  message(STATUS "Windows: skipping conda extension copy (bundled Python has its own DLLs)")

  # Copy specific conda DLLs needed by Python's .pyd extensions to the build
  # output directory. Conda's .pyd extensions (pyexpat, _ssl, etc.) have implicit
  # DLL dependencies in Library/bin/. We only copy the Python-specific ones to
  # avoid conflicting with Qt/VTK/Boost DLLs already in the build directory.
  set(_conda_lib_bin "${_conda_prefix}/Library/bin")
  set(_python_dep_dlls
    libexpat.dll        # pyexpat.pyd
    libssl-3-x64.dll    # _ssl.pyd
    libcrypto-3-x64.dll # _hashlib.pyd
    libffi-8.dll        # _ctypes.pyd (if present)
    zlib.dll            # zlib module
    libbz2.dll          # _bz2.pyd
    liblzma.dll         # _lzma.pyd
    sqlite3.dll         # _sqlite3.pyd
  )
  foreach(_dll ${_python_dep_dlls})
    if(EXISTS "${_conda_lib_bin}/${_dll}")
      message(STATUS "Copying ${_dll} from conda Library/bin to build output")
      file(COPY "${_conda_lib_bin}/${_dll}" DESTINATION "${CMAKE_BINARY_DIR}/bin/Release")
      file(COPY "${_conda_lib_bin}/${_dll}" DESTINATION "${CMAKE_BINARY_DIR}/bin")
    endif()
  endforeach()

  # Copy VTK DLLs to build output for build-tree testing (shared VTK only).
  foreach(_pp ${CMAKE_PREFIX_PATH})
    file(GLOB _vtk_dlls "${_pp}/bin/vtk*-9.5.dll")
    if(_vtk_dlls)
      list(LENGTH _vtk_dlls _n)
      message(STATUS "Copying ${_n} VTK DLLs from ${_pp}/bin to build output")
      foreach(_dll ${_vtk_dlls})
        get_filename_component(_name "${_dll}" NAME)
        file(COPY "${_dll}" DESTINATION "${CMAKE_BINARY_DIR}/bin/Release")
        file(COPY "${_dll}" DESTINATION "${CMAKE_BINARY_DIR}/bin")
      endforeach()
    endif()
  endforeach()
else()
  get_filename_component(_conda_prefix "${_conda_prefix}" DIRECTORY)       # .../
  set(_conda_lib_dynload "${_conda_prefix}/lib/python3.12/lib-dynload")
  set(_bundled_lib_dynload "${BUNDLED_PYTHON_ROOT}/lib/python3.12/lib-dynload")

  if(EXISTS "${_conda_lib_dynload}" AND IS_DIRECTORY "${_conda_lib_dynload}")
    file(GLOB _conda_ext_files "${_conda_lib_dynload}/*.so" "${_conda_lib_dynload}/*.dylib")
    list(LENGTH _conda_ext_files _num_ext)
    message(STATUS "Copying ${_num_ext} extension modules from conda to bundled Python")
    file(COPY ${_conda_ext_files} DESTINATION "${_bundled_lib_dynload}")
  else()
    message(WARNING "Conda extension directory not found at ${_conda_lib_dynload}")
  endif()
endif()

# ---------------------------------------------------------------------------
# Compile-time defines so C++ can detect bundled Python
# ---------------------------------------------------------------------------
add_definitions(-DSHAPEWORKS_BUNDLED_PYTHON)

# Provide build-tree paths so PythonWorker can find shapeworks Python modules
# and shapeworks_py.so during development (not needed in installed app).
if(WIN32)
  # On Windows, build output goes to bin/Release or bin/Debug
  add_compile_definitions(SHAPEWORKS_BUNDLED_PYTHON_BUILD_BIN="${CMAKE_BINARY_DIR}/bin/Release")
else()
  add_compile_definitions(SHAPEWORKS_BUNDLED_PYTHON_BUILD_BIN="${CMAKE_BINARY_DIR}/bin")
endif()
add_compile_definitions(SHAPEWORKS_BUNDLED_PYTHON_SOURCE_DIR="${CMAKE_SOURCE_DIR}/Python")
add_compile_definitions(SHAPEWORKS_BUNDLED_PYTHON_CONDA_PREFIX="${_conda_prefix}")

# ---------------------------------------------------------------------------
# In-source ShapeWorks Python packages — editable-installed into the bundled
# Python so 'swpython RunUseCase.py' against a source build picks up edits to
# Python/shapeworks etc. without rebuilding. The install rules copy these
# directories in separately for the deployed bundle.
# ---------------------------------------------------------------------------
set(_sw_editable_packages
  "${CMAKE_SOURCE_DIR}/Python/shapeworks"
  "${CMAKE_SOURCE_DIR}/Python/DataAugmentationUtilsPackage"
  "${CMAKE_SOURCE_DIR}/Python/DatasetUtilsPackage"
  "${CMAKE_SOURCE_DIR}/Python/DeepSSMUtilsPackage"
  "${CMAKE_SOURCE_DIR}/Python/DocumentationUtilsPackage"
  "${CMAKE_SOURCE_DIR}/Python/ShapeCohortGenPackage"
)

# ---------------------------------------------------------------------------
# Custom target: install pip packages into the bundled Python
# ---------------------------------------------------------------------------
if(WIN32)
  # On Windows GHA runners, pip's platformdirs crashes trying to read
  # CSIDL_COMMON_APPDATA from the registry. The --isolated flag skips
  # all config file and environment variable processing.
  set(_pip_flags --isolated)
else()
  set(_pip_flags "")
endif()

# Detect dep-built (shared) VTK Python wrapping in CMAKE_PREFIX_PATH. When present,
# provision it into the bundled Python (+ a stub vtk dist-info) BEFORE pip runs, so
# transitive vtk dependents (pyvista) resolve to our build instead of pulling the
# ~379 MB pip vtk wheel. (BundledPython.cmake is included before find_package(VTK),
# so we glob CMAKE_PREFIX_PATH rather than use VTK_DIR.)
set(_dep_vtk_sp "")
# Primary: derive from VTK_DIR. CI passes -DVTK_DIR=<deps>/lib/cmake/vtk-9.5
# explicitly (the deps install is NOT on CMAKE_PREFIX_PATH there — only conda
# is), and -D cache vars are set before this file is included. Our from-source
# VTK installs its Python wrapping at <prefix>/lib/python3.12/site-packages
# (Linux/macOS) or <prefix>/lib/site-packages (Windows).
if(DEFINED VTK_DIR AND VTK_DIR)
  get_filename_component(_cand "${VTK_DIR}/../../python3.12/site-packages" ABSOLUTE)
  if(EXISTS "${_cand}/vtkmodules")
    set(_dep_vtk_sp "${_cand}")
  endif()
  if(NOT _dep_vtk_sp)
    get_filename_component(_cand "${VTK_DIR}/../../site-packages" ABSOLUTE)
    if(EXISTS "${_cand}/vtkmodules")
      set(_dep_vtk_sp "${_cand}")
    endif()
  endif()
endif()
# Fallback for local dev that puts the deps install on CMAKE_PREFIX_PATH
# instead of passing -DVTK_DIR. Require a CMake package config (lib/cmake/vtk-*)
# so we match our from-source VTK, never a conda/pip vtk wheel (which ships no
# CMake config and whose vtkmodules need auditwheel-vendored libs we don't bundle).
if(NOT _dep_vtk_sp)
  foreach(_pp ${CMAKE_PREFIX_PATH})
    file(GLOB _vtk_cmake_cfg "${_pp}/lib/cmake/vtk-*")
    if(_vtk_cmake_cfg)
      if(EXISTS "${_pp}/lib/python3.12/site-packages/vtkmodules")
        set(_dep_vtk_sp "${_pp}/lib/python3.12/site-packages")
      elseif(EXISTS "${_pp}/lib/site-packages/vtkmodules")
        set(_dep_vtk_sp "${_pp}/lib/site-packages")
      endif()
    endif()
  endforeach()
endif()
if(WIN32)
  set(_bundled_site_packages "${BUNDLED_PYTHON_ROOT}/Lib/site-packages")
else()
  set(_bundled_site_packages "${BUNDLED_PYTHON_ROOT}/lib/python3.12/site-packages")
endif()
set(_dep_vtk_version "9.5.0")  # matches VTK_VER in build_dependencies.sh

# Detect dep-built (shared) ITK Python wrapping. Same VTK_DIR-trick as above:
# CI passes -DITK_DIR=<deps>/lib/cmake/ITK-5.4 explicitly; deps install isn't on
# CMAKE_PREFIX_PATH there. ITK's Python wrapping lands at
# <prefix>/lib/python3.12/site-packages (Linux/macOS) — same layout as VTK.
set(_dep_itk_sp "")
if(DEFINED ITK_DIR AND ITK_DIR)
  get_filename_component(_cand "${ITK_DIR}/../../python3.12/site-packages" ABSOLUTE)
  if(EXISTS "${_cand}/itk")
    set(_dep_itk_sp "${_cand}")
  endif()
  if(NOT _dep_itk_sp)
    get_filename_component(_cand "${ITK_DIR}/../../site-packages" ABSOLUTE)
    if(EXISTS "${_cand}/itk")
      set(_dep_itk_sp "${_cand}")
    endif()
  endif()
endif()
# Local-dev fallback for when the deps install is on CMAKE_PREFIX_PATH instead.
# Require lib/cmake/ITK-* so we match our from-source build, never a pip itk
# wheel (which ships no CMake configs and is statically linked / monolithic).
if(NOT _dep_itk_sp)
  foreach(_pp ${CMAKE_PREFIX_PATH})
    file(GLOB _itk_cmake_cfg "${_pp}/lib/cmake/ITK-*")
    if(_itk_cmake_cfg)
      if(EXISTS "${_pp}/lib/python3.12/site-packages/itk")
        set(_dep_itk_sp "${_pp}/lib/python3.12/site-packages")
      elseif(EXISTS "${_pp}/lib/site-packages/itk")
        set(_dep_itk_sp "${_pp}/lib/site-packages")
      endif()
    endif()
  endforeach()
endif()
set(_dep_itk_version "5.4.4.post1")  # matches the version in python_requirements.txt

set(_provision_cmds)
if(_dep_vtk_sp)
  message(STATUS "Dep-built VTK detected at ${_dep_vtk_sp} — will provision into bundled Python")
  list(APPEND _provision_cmds COMMAND "${BUNDLED_PYTHON_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/cmake/provision_bundled_vtk.py" "${_dep_vtk_sp}" "${_bundled_site_packages}" "${_dep_vtk_version}")
endif()
if(_dep_itk_sp)
  message(STATUS "Dep-built ITK detected at ${_dep_itk_sp} — will provision into bundled Python")
  list(APPEND _provision_cmds COMMAND "${BUNDLED_PYTHON_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/cmake/provision_bundled_itk.py" "${_dep_itk_sp}" "${_bundled_site_packages}" "${_dep_itk_version}")
endif()

add_custom_target(bundled_pip_install
  ${_provision_cmds}
  COMMAND "${BUNDLED_PYTHON_EXECUTABLE}" -m pip install ${_pip_flags} --upgrade pip
  COMMAND "${BUNDLED_PYTHON_EXECUTABLE}" -m pip install ${_pip_flags} -r "${CMAKE_SOURCE_DIR}/python_requirements_bundled.txt"
  COMMENT "Provisioning dep VTK/ITK + installing runtime pip packages into bundled Python"
  VERBATIM
)

foreach(_pkg ${_sw_editable_packages})
  if(EXISTS "${_pkg}/setup.py")
    add_custom_command(TARGET bundled_pip_install POST_BUILD
      COMMAND "${BUNDLED_PYTHON_EXECUTABLE}" -m pip install ${_pip_flags} --no-deps -e "${_pkg}"
      COMMENT "Editable-installing ${_pkg} into bundled Python"
      VERBATIM
    )
  endif()
endforeach()

# ---------------------------------------------------------------------------
# Patch SyntaxWarnings out of upstream pip packages that we ship. mbpls still
# uses `is not '<literal>'` which Python 3.12 warns about on every import.
# Patching the installed copy keeps the deployed bundle quiet without forking
# the upstream package. Idempotent — skips files already patched. Extend the
# patches list below for new noisy packages.
# ---------------------------------------------------------------------------
set(_patch_script "${CMAKE_BINARY_DIR}/_bundled_python/patch_third_party.py")
file(WRITE "${_patch_script}"
[=[
"""Patch SyntaxWarning-emitting upstream packages installed into bundled Python."""
import os
import sys
import sysconfig

PATCHES = [
    # (relative_path_under_site_packages, [(old_text, new_text), ...])
    ("mbpls/mbpls.py", [("is not 'SIMPLS'", "!= 'SIMPLS'")]),
]

site_packages = sysconfig.get_paths()["purelib"]
for rel_path, replacements in PATCHES:
    path = os.path.join(site_packages, rel_path)
    if not os.path.exists(path):
        continue
    with open(path, "r", encoding="utf-8") as f:
        content = f.read()
    new_content = content
    for old, new in replacements:
        new_content = new_content.replace(old, new)
    if new_content != content:
        with open(path, "w", encoding="utf-8") as f:
            f.write(new_content)
        print(f"Patched {path}", file=sys.stderr)
]=])

add_custom_command(TARGET bundled_pip_install POST_BUILD
  COMMAND "${BUNDLED_PYTHON_EXECUTABLE}" "${_patch_script}"
  COMMENT "Patching SyntaxWarning fixes into upstream pip packages"
  VERBATIM
)

# ---------------------------------------------------------------------------
# Build-tree swpython wrapper — points at the bundled python in the build dir
# and puts CMAKE_RUNTIME_OUTPUT_DIRECTORY on PYTHONPATH so shapeworks_py.so is
# importable. The install-tree variant is generated in InstallBundledPython.cmake.
# ---------------------------------------------------------------------------
if(WIN32)
  set(_dev_deps_bin "")
  foreach(_pp ${CMAKE_PREFIX_PATH})
    if(IS_DIRECTORY "${_pp}/bin")
      set(_dev_deps_bin "${_dev_deps_bin}${_pp}/bin;")
    endif()
  endforeach()
  set(_dev_wrapper "${CMAKE_BINARY_DIR}/bin/swpython.bat")
  file(WRITE "${_dev_wrapper}"
"@echo off
set \"PYTHONHOME=${BUNDLED_PYTHON_ROOT}\"
set \"PYTHONPATH=${CMAKE_BINARY_DIR}/bin/Release;${CMAKE_BINARY_DIR}/bin\"
set \"PATH=${CMAKE_BINARY_DIR}/bin/Release;${CMAKE_BINARY_DIR}/bin;${_dev_deps_bin}%PATH%\"
set \"SW_DLL_DIRS=${CMAKE_BINARY_DIR}/bin/Release;${CMAKE_BINARY_DIR}/bin\"
if not defined MPLBACKEND set \"MPLBACKEND=Agg\"
\"${BUNDLED_PYTHON_EXECUTABLE}\" %*
")
  # Write sitecustomize.py to register DLL directories from SW_DLL_DIRS
  file(WRITE "${BUNDLED_PYTHON_ROOT}/Lib/sitecustomize.py"
"import os, sys
if sys.platform == 'win32' and hasattr(os, 'add_dll_directory'):
    for d in os.environ.get('SW_DLL_DIRS', '').split(';'):
        if d and os.path.isdir(d):
            os.add_dll_directory(d)
")
else()
  set(_dev_wrapper "${CMAKE_BINARY_DIR}/bin/swpython")
  file(WRITE "${_dev_wrapper}"
"#!/bin/bash
export PYTHONHOME='${BUNDLED_PYTHON_ROOT}'
export PYTHONPATH='${CMAKE_BINARY_DIR}/bin'
export PATH='${CMAKE_BINARY_DIR}/bin':\"\$PATH\"
export MPLBACKEND=\"\${MPLBACKEND:-Agg}\"
exec '${BUNDLED_PYTHON_EXECUTABLE}' \"\$@\"
")
  execute_process(COMMAND chmod 755 "${_dev_wrapper}")
endif()
message(STATUS "Build-tree swpython wrapper: ${_dev_wrapper}")

# Build-tree swpip wrapper — delegates to swpython -m shapeworks.swpip
if(WIN32)
  set(_dev_swpip "${CMAKE_BINARY_DIR}/bin/swpip.bat")
  file(WRITE "${_dev_swpip}"
"@echo off
call \"${CMAKE_BINARY_DIR}/bin/swpython.bat\" -m shapeworks.swpip %*
")
else()
  set(_dev_swpip "${CMAKE_BINARY_DIR}/bin/swpip")
  file(WRITE "${_dev_swpip}"
"#!/bin/bash
exec '${CMAKE_BINARY_DIR}/bin/swpython' -m shapeworks.swpip \"\$@\"
")
  execute_process(COMMAND chmod 755 "${_dev_swpip}")
endif()

message(STATUS "Bundled Python root: ${BUNDLED_PYTHON_ROOT}")
message(STATUS "Bundled Python executable: ${BUNDLED_PYTHON_EXECUTABLE}")
