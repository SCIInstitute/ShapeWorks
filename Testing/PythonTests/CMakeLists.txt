set(TEST_SRCS
  PythonTests.cpp
  )

add_executable(PythonTests
  ${TEST_SRCS}
  )

target_link_libraries(PythonTests
  Testing
  )

add_test(NAME PythonTests COMMAND PythonTests)
