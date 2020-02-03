set(TEST_SRCS
        ParticlesTests.cpp
        )

add_executable(ParticlesTests
        ${TEST_SRCS}
        )

target_link_libraries(ParticlesTests
        ${ITK_LIBRARIES} ${VTK_LIBRARIES}
        tinyxml Mesh vgl vgl_algo Optimize Utils trimesh2 Particles
        gtest_main)

add_test(NAME ParticlesTests COMMAND ParticlesTests)
