set(TEST_SRCS
  MeshTests.cpp
  )

add_executable(MeshTests
  ${TEST_SRCS}
  )

target_link_libraries(MeshTests
  tinyxml Optimize Mesh Utils Particles trimesh2 Eigen3::Eigen igl::core
  ${ITK_LIBRARIES} ${VTK_LIBRARIES}
  Testing Project
  )

add_test(NAME MeshTests COMMAND MeshTests)
