
set(TEST_SRCS
  PythonTests.cpp
  )

add_executable(PythonTests
  ${TEST_SRCS}
  )

target_link_libraries(PythonTests
  ${ITK_LIBRARIES} ${VTK_LIBRARIES}
  GTest::Main
  )

add_test(NAME PythonTests COMMAND PythonTests)
