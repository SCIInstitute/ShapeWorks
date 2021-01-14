set(TEST_SRCS
  MeshTests.cpp
  )

add_executable(MeshTests
  ${TEST_SRCS}
  )

target_link_libraries(MeshTests
  tinyxml Mesh vgl vgl_algo Optimize Utils trimesh2 Eigen3::Eigen igl::core
  ${ITK_LIBRARIES} ${VTK_LIBRARIES}
  Testing
  )


add_test(NAME MeshTests COMMAND MeshTests)
