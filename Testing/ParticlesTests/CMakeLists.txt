set(TEST_SRCS
  ParticlesTests.cpp
  )

add_executable(ParticlesTests
  ${TEST_SRCS}
  )

target_link_libraries(ParticlesTests
  Mesh Optimize Utils trimesh2 Particles
  Testing pybind11::embed Project Image
  )

add_test(NAME ParticlesTests COMMAND ParticlesTests)
