
set(TEST_SRCS
  PythonTests.cpp
  )

add_executable(PythonTests
  ${TEST_SRCS}
  )

target_link_libraries(PythonTests
  GTest::Main
  )

add_test(NAME PythonTests COMMAND PythonTests)
