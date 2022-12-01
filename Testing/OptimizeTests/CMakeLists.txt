set(TEST_SRCS
  OptimizeTests.cpp
  )

add_executable(OptimizeTests
  ${TEST_SRCS}
  )

target_link_libraries(OptimizeTests
  ${ITK_LIBRARIES} ${VTK_LIBRARIES}
  tinyxml Mesh Optimize Utils trimesh2 Particles
  Testing pybind11::embed Image Project
  )

add_test(NAME OptimizeTests COMMAND OptimizeTests)
