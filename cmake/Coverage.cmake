# Test coverage support for C++ (gcov format, via gcovr) and Python (coverage.py).
#
# Usage:
#   cmake -DSW_ENABLE_COVERAGE=ON ...
#   ninja                    # build instrumented binaries
#   ninja coverage           # clean counters, run ctest, write reports
#
# Reports are written to <build>/coverage/{cpp,python}/index.html.
# Extra ctest arguments can be passed in the SW_COVERAGE_CTEST_ARGS environment
# variable, e.g. SW_COVERAGE_CTEST_ARGS="-R Mesh" ninja coverage
#
# Include this after ExternalLibs so third-party code is not instrumented.

if(MSVC)
  message(FATAL_ERROR "SW_ENABLE_COVERAGE is only supported with gcc or clang")
endif()

message(STATUS "Coverage instrumentation enabled")

# -O0 keeps line counts accurate; it comes after the build-type flags so it wins
add_compile_options(--coverage -O0 -g)
add_link_options(--coverage)

if(APPLE)
  execute_process(COMMAND xcrun -f llvm-cov OUTPUT_VARIABLE LLVM_COV OUTPUT_STRIP_TRAILING_WHITESPACE)
  set(SW_GCOV_DEFAULT "${LLVM_COV} gcov")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(SW_GCOV_DEFAULT "llvm-cov gcov")
else()
  set(SW_GCOV_DEFAULT "gcov")
endif()
set(SW_GCOV_EXECUTABLE "${SW_GCOV_DEFAULT}" CACHE STRING "gcov command used by gcovr")

find_program(GCOVR_EXECUTABLE gcovr)
if(NOT GCOVR_EXECUTABLE)
  message(WARNING "gcovr not found: run 'pip install gcovr' and re-run cmake to get C++ coverage reports")
endif()

set(SW_COVERAGE_DIR "${CMAKE_BINARY_DIR}/coverage")
set(SW_COVERAGE_SUPPORT_DIR "${CMAKE_BINARY_DIR}/CoverageSupport")

# Python coverage: sitecustomize.py is placed on PYTHONPATH while the tests run,
# so every Python process they launch starts coverage.py
set(SW_PYTHON_SRC "${CMAKE_SOURCE_DIR}/Python")
file(WRITE "${SW_COVERAGE_SUPPORT_DIR}/startup/sitecustomize.py"
"try:
    import coverage
    coverage.process_startup()
except ImportError:
    pass
")
file(WRITE "${SW_COVERAGE_SUPPORT_DIR}/coveragerc"
"[run]
data_file = ${SW_COVERAGE_SUPPORT_DIR}/data/.coverage
parallel = True
concurrency = thread,multiprocessing
source =
    ${SW_PYTHON_SRC}/shapeworks/shapeworks
    ${SW_PYTHON_SRC}/DataAugmentationUtilsPackage/DataAugmentationUtils
    ${SW_PYTHON_SRC}/DatasetUtilsPackage/DatasetUtils
    ${SW_PYTHON_SRC}/DeepSSMUtilsPackage/DeepSSMUtils
    ${SW_PYTHON_SRC}/ShapeCohortGenPackage/ShapeCohortGen

[report]
skip_empty = True
")

set(SW_COVERAGE_ARGS
  -DBUILD_DIR=${CMAKE_BINARY_DIR}
  -DSOURCE_DIR=${CMAKE_SOURCE_DIR}
  -DCOVERAGE_DIR=${SW_COVERAGE_DIR}
  -DSUPPORT_DIR=${SW_COVERAGE_SUPPORT_DIR}
  -DGCOVR=${GCOVR_EXECUTABLE}
  "-DGCOV=${SW_GCOV_EXECUTABLE}"
  -DCTEST=${CMAKE_CTEST_COMMAND})

add_custom_target(coverage_clean
  COMMAND ${CMAKE_COMMAND} ${SW_COVERAGE_ARGS} -DSTEP=clean -P ${CMAKE_SOURCE_DIR}/cmake/CoverageSteps.cmake
  VERBATIM)

add_custom_target(coverage_report
  COMMAND ${CMAKE_COMMAND} ${SW_COVERAGE_ARGS} -DSTEP=report -P ${CMAKE_SOURCE_DIR}/cmake/CoverageSteps.cmake
  VERBATIM)

add_custom_target(coverage
  COMMAND ${CMAKE_COMMAND} ${SW_COVERAGE_ARGS} -DSTEP=all -P ${CMAKE_SOURCE_DIR}/cmake/CoverageSteps.cmake
  USES_TERMINAL
  VERBATIM)
