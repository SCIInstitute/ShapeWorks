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
else()
  message(FATAL_ERROR "USE_BUNDLED_PYTHON: unsupported platform (Windows not yet supported)")
endif()

set(_archive_name "cpython-${BUNDLED_PYTHON_VERSION}+${BUNDLED_PYTHON_RELEASE}-${_platform_tag}-install_only_stripped.tar.gz")
set(_download_url "https://github.com/astral-sh/python-build-standalone/releases/download/${BUNDLED_PYTHON_RELEASE}/${_archive_name}")

set(_bundled_dir "${CMAKE_BINARY_DIR}/_bundled_python")
set(_archive_path "${_bundled_dir}/${_archive_name}")
set(BUNDLED_PYTHON_ROOT "${_bundled_dir}/python" CACHE PATH "Root of bundled standalone Python" FORCE)

if(APPLE)
  set(BUNDLED_PYTHON_EXECUTABLE "${BUNDLED_PYTHON_ROOT}/bin/python3" CACHE FILEPATH "Bundled Python executable" FORCE)
else()
  set(BUNDLED_PYTHON_EXECUTABLE "${BUNDLED_PYTHON_ROOT}/bin/python3" CACHE FILEPATH "Bundled Python executable" FORCE)
endif()

# ---------------------------------------------------------------------------
# Download + extract (only if not already present)
# ---------------------------------------------------------------------------
if(NOT EXISTS "${BUNDLED_PYTHON_ROOT}/bin/python3")
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
if(NOT EXISTS "${BUNDLED_PYTHON_ROOT}/bin/python3")
  message(FATAL_ERROR "Bundled Python extraction failed — ${BUNDLED_PYTHON_ROOT}/bin/python3 not found")
endif()

# ---------------------------------------------------------------------------
# Copy conda's lib-dynload into the bundled tree.
# The build-time embedded interpreter uses conda's libpython, which expects
# C extension modules (math, _json, etc.) as .so files in lib-dynload/.
# python-build-standalone compiles these statically into its libpython,
# leaving lib-dynload nearly empty. Copying conda's extensions ensures
# the interpreter works in both the build tree and the installed .app.
# ---------------------------------------------------------------------------
find_package(Python3 3.12 REQUIRED COMPONENTS Interpreter)
get_filename_component(_conda_prefix "${Python3_EXECUTABLE}" DIRECTORY)  # .../bin
get_filename_component(_conda_prefix "${_conda_prefix}" DIRECTORY)       # .../
set(_conda_lib_dynload "${_conda_prefix}/lib/python3.12/lib-dynload")
set(_bundled_lib_dynload "${BUNDLED_PYTHON_ROOT}/lib/python3.12/lib-dynload")

if(EXISTS "${_conda_lib_dynload}" AND IS_DIRECTORY "${_conda_lib_dynload}")
  file(GLOB _conda_so_files "${_conda_lib_dynload}/*.so" "${_conda_lib_dynload}/*.dylib")
  list(LENGTH _conda_so_files _num_so)
  message(STATUS "Copying ${_num_so} extension modules from conda lib-dynload to bundled Python")
  file(COPY ${_conda_so_files} DESTINATION "${_bundled_lib_dynload}")
else()
  message(WARNING "Conda lib-dynload not found at ${_conda_lib_dynload}")
endif()

# ---------------------------------------------------------------------------
# Compile-time defines so C++ can detect bundled Python
# ---------------------------------------------------------------------------
add_definitions(-DSHAPEWORKS_BUNDLED_PYTHON)

# Provide build-tree paths so PythonWorker can find shapeworks Python modules
# and shapeworks_py.so during development (not needed in installed .app).
add_compile_definitions(SHAPEWORKS_BUNDLED_PYTHON_BUILD_BIN="${CMAKE_BINARY_DIR}/bin")
add_compile_definitions(SHAPEWORKS_BUNDLED_PYTHON_SOURCE_DIR="${CMAKE_SOURCE_DIR}/Python")
add_compile_definitions(SHAPEWORKS_BUNDLED_PYTHON_CONDA_PREFIX="${_conda_prefix}")

# ---------------------------------------------------------------------------
# Custom target: install pip packages into the bundled Python
# ---------------------------------------------------------------------------
add_custom_target(bundled_pip_install
  COMMAND "${BUNDLED_PYTHON_EXECUTABLE}" -m pip install --upgrade pip
  COMMAND "${BUNDLED_PYTHON_EXECUTABLE}" -m pip install -r "${CMAKE_SOURCE_DIR}/python_requirements_bundled.txt"
  COMMENT "Installing runtime pip packages into bundled Python"
  VERBATIM
)

message(STATUS "Bundled Python root: ${BUNDLED_PYTHON_ROOT}")
message(STATUS "Bundled Python executable: ${BUNDLED_PYTHON_EXECUTABLE}")
