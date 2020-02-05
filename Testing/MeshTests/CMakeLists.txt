

set(TEST_SRCS
  MeshTests.cpp
  )

add_executable(MeshTests
  ${TEST_SRCS}
  )

target_link_libraries(MeshTests
  tinyxml Mesh vgl vgl_algo Mesh Optimize Utils trimesh2
  gtest_main ${ITK_LIBRARIES} ${VTK_LIBRARIES})

add_test(NAME MeshTests COMMAND MeshTests)
