
set(TEST_SRCS
  PythonTests.cpp
  )

add_executable(PythonTests
  ${TEST_SRCS}
  )

target_link_libraries(PythonTests
  gtest_main )

add_test(NAME PythonTests COMMAND PythonTests)
