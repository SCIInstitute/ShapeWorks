set(TEST_SRCS
  OptimizeTests.cpp
  )

add_executable(OptimizeTests
  ${TEST_SRCS}
  )

target_link_libraries(OptimizeTests
  ${ITK_LIBRARIES} ${VTK_LIBRARIES}
  tinyxml Mesh vgl vgl_algo Optimize Utils trimesh2 Particles
  Testing
  )

add_test(NAME OptimizeTests COMMAND OptimizeTests)
