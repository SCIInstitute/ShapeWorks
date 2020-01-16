

set(TEST_SRCS
  PythonTests.cc
  )

set(TEST_HDRS
  PythonTests.h
  )

add_executable(PythonTests
  ${TEST_SRCS}
  )

target_link_libraries(PythonTests
  tinyxml Mesh vgl vgl_algo Image Optimize Utils trimesh2
  gtest_main ${ITK_LIBRARIES} ${VTK_LIBRARIES})

add_test(NAME PythonTests COMMAND PythonTests)
