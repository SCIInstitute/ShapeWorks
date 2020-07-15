set(TEST_SRCS
  PythonTests.cpp
  )

add_executable(PythonTests
  ${TEST_SRCS}
  )

target_link_libraries(PythonTests
  ${ITK_LIBRARIES} ${VTK_LIBRARIES}
  Testing
  )

add_test(NAME PythonTests COMMAND PythonTests)
