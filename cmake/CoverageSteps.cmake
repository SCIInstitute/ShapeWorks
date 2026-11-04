# Coverage steps, run with cmake -P from the targets in Coverage.cmake.
# STEP is one of: clean, report, all (clean + ctest + report).

function(coverage_clean)
  file(GLOB_RECURSE gcda_files "${BUILD_DIR}/*.gcda")
  list(LENGTH gcda_files count)
  if(count GREATER 0)
    file(REMOVE ${gcda_files})
  endif()
  file(REMOVE_RECURSE "${SUPPORT_DIR}/data" "${COVERAGE_DIR}")
  message(STATUS "Removed ${count} .gcda files and previous Python coverage data")
endfunction()

function(coverage_test)
  file(MAKE_DIRECTORY "${SUPPORT_DIR}/data")

  find_program(PYTHON NAMES python python3)
  execute_process(COMMAND ${PYTHON} -c "import coverage" RESULT_VARIABLE has_coverage OUTPUT_QUIET ERROR_QUIET)
  if(has_coverage EQUAL 0)
    set(ENV{PYTHONPATH} "${SUPPORT_DIR}/startup:$ENV{PYTHONPATH}")
    set(ENV{COVERAGE_PROCESS_START} "${SUPPORT_DIR}/coveragerc")
  else()
    message(WARNING "Python package 'coverage' not found: run 'pip install coverage' to get Python coverage")
  endif()

  separate_arguments(extra_args UNIX_COMMAND "$ENV{SW_COVERAGE_CTEST_ARGS}")
  execute_process(COMMAND ${CTEST} --output-on-failure ${extra_args}
                  WORKING_DIRECTORY "${BUILD_DIR}"
                  RESULT_VARIABLE result)
  set(ctest_result ${result} PARENT_SCOPE)
endfunction()

function(coverage_report)
  cmake_host_system_information(RESULT cores QUERY NUMBER_OF_LOGICAL_CORES)

  find_program(PYTHON NAMES python python3)

  # C++
  if(GCOVR)
    file(MAKE_DIRECTORY "${COVERAGE_DIR}/cpp")
    message(STATUS "Generating C++ coverage report")
    execute_process(
      COMMAND ${GCOVR}
              --root ${SOURCE_DIR}
              --object-directory ${BUILD_DIR}
              --gcov-executable ${GCOV}
              --filter ${SOURCE_DIR}/Libs/
              --filter ${SOURCE_DIR}/Applications/
              --filter ${SOURCE_DIR}/Studio/
              --exclude-unreachable-branches
              --exclude-throw-branches
              # very hot loops legitimately exceed gcovr's "suspicious" hit count
              --gcov-ignore-parse-errors=negative_hits.warn_once_per_file
              --gcov-ignore-parse-errors=suspicious_hits.warn_once_per_file
              --merge-mode-functions=separate
              -j ${cores}
              --html-nested ${COVERAGE_DIR}/cpp/index.html
              --cobertura ${COVERAGE_DIR}/cpp/coverage.xml
              --json-summary ${COVERAGE_DIR}/cpp/summary.json
      WORKING_DIRECTORY "${BUILD_DIR}"
      RESULT_VARIABLE result)
    if(NOT result EQUAL 0)
      set(report_failed TRUE PARENT_SCOPE)
      message(SEND_ERROR "gcovr failed")
    endif()
  else()
    message(WARNING "gcovr not found: skipping C++ coverage report")
  endif()

  # Python (run from the source dir so the report shows relative paths)
  set(data "${SUPPORT_DIR}/data")
  set(rc "--rcfile=${SUPPORT_DIR}/coveragerc")
  file(GLOB parallel_files "${data}/.coverage.*")
  if(parallel_files OR EXISTS "${data}/.coverage")
    message(STATUS "Generating Python coverage report")
    if(parallel_files)
      execute_process(COMMAND ${PYTHON} -m coverage combine -q ${rc})
    endif()
    file(MAKE_DIRECTORY "${COVERAGE_DIR}/python")
    foreach(args "html;-d;${COVERAGE_DIR}/python"
                 "xml;-o;${COVERAGE_DIR}/python/coverage.xml"
                 "json;-o;${COVERAGE_DIR}/python/coverage.json")
      execute_process(COMMAND ${PYTHON} -m coverage ${args} -q ${rc} WORKING_DIRECTORY "${SOURCE_DIR}")
    endforeach()
  else()
    message(STATUS "No Python coverage data found")
  endif()

  # Combined summary: coverage/index.html, coverage/summary.json
  execute_process(COMMAND ${PYTHON} ${SOURCE_DIR}/cmake/coverage_summary.py ${COVERAGE_DIR})
endfunction()

if(STEP STREQUAL "clean")
  coverage_clean()
elseif(STEP STREQUAL "report")
  coverage_report()
  if(report_failed)
    message(FATAL_ERROR "Coverage report failed")
  endif()
elseif(STEP STREQUAL "all")
  coverage_clean()
  coverage_test()
  coverage_report()
  message(STATUS "Summary: ${COVERAGE_DIR}/index.html")
  if(report_failed)
    message(FATAL_ERROR "Coverage report failed")
  endif()
  if(NOT ctest_result EQUAL 0)
    message(FATAL_ERROR "Some tests failed (coverage reports were still written)")
  endif()
else()
  message(FATAL_ERROR "Unknown coverage step: ${STEP}")
endif()
