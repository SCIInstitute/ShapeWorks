set(TEST_SRCS
  ParticlesTests.cpp
  )

add_executable(ParticlesTests
  ${TEST_SRCS}
  )

target_link_libraries(ParticlesTests
  Testing Mesh Optimize Utils trimesh2 Particles
  pybind11::embed Project Image
  )

add_test(NAME ParticlesTests COMMAND ParticlesTests)
