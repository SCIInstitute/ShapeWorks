set(TEST_SRCS
  OptimizeTests.cpp
  )

add_executable(OptimizeTests
  ${TEST_SRCS}
  )

target_link_libraries(OptimizeTests
  Mesh Optimize Utils trimesh2 Particles
  Testing pybind11::embed Project Image
  )

add_test(NAME OptimizeTests COMMAND OptimizeTests)

