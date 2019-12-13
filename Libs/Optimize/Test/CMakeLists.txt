
# Set the test data location
set(TEST_DATA_DIR "${CMAKE_SOURCE_DIR}/Testing")

# Configure the TestConfiguration.h file
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/TestConfiguration.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/TestConfiguration.h
  @ONLY )

include_directories(BEFORE ${CMAKE_CURRENT_BINARY_DIR})

set(TEST_SRCS
  OptimizeTests.cc
  )
set(TEST_HDRS
  OptimizeTests.h
  )

add_executable(OptimizeTests
  ${TEST_SRCS}
  )

target_link_libraries(OptimizeTests
  ${ITK_LIBRARIES} ${VTK_LIBRARIES}
  tinyxml Mesh vgl vgl_algo Optimize Utils trimesh2
  gtest_main)

add_test(NAME OptimizeTests COMMAND OptimizeTests)
