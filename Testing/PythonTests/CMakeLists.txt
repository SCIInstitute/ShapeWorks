set(TEST_SRCS
  PythonTests.cpp
  )

add_executable(PythonTests
  ${TEST_SRCS}
  )

target_link_libraries(PythonTests
  Testing
  )

# Each Python test starts its own interpreter, and most of its runtime is importing
# shapeworks. Split them into GoogleTest shards so that `ctest -j` runs them in parallel.
set(PYTHON_TEST_SHARDS 4)
math(EXPR LAST_PYTHON_TEST_SHARD "${PYTHON_TEST_SHARDS} - 1")
foreach(SHARD RANGE ${LAST_PYTHON_TEST_SHARD})
  add_test(NAME PythonTests_shard${SHARD} COMMAND PythonTests)
  set_tests_properties(PythonTests_shard${SHARD} PROPERTIES
    ENVIRONMENT "GTEST_TOTAL_SHARDS=${PYTHON_TEST_SHARDS};GTEST_SHARD_INDEX=${SHARD}")
endforeach()
