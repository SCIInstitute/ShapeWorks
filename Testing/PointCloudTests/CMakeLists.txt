set(TEST_SRCS
  PointCloudTests.cpp
  )

add_executable(PointCloudTests
  ${TEST_SRCS}
  )

target_link_libraries(PointCloudTests
  PointCloud
  ${ITK_LIBRARIES} ${VTK_LIBRARIES}
  tinyxml Mesh vgl vgl_algo Optimize Utils trimesh2
  gtest_main Eigen3::Eigen)

add_test(NAME PointCloudTests COMMAND PointCloudTests)
