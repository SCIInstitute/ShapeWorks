set(TEST_SRCS
  MeshTests.cpp
  )

add_executable(MeshTests
  ${TEST_SRCS}
  )

target_link_libraries(MeshTests
  Testing tinyxml Optimize Mesh Utils Particles trimesh2 Eigen3::Eigen igl::core
  ${ITK_LIBRARIES} ${VTK_LIBRARIES}
  Project
  )

add_test(NAME MeshTests COMMAND MeshTests)
