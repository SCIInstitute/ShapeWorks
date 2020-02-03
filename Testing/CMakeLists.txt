# Set the test data location
set(TEST_DATA_DIR "${CMAKE_SOURCE_DIR}/Testing/data")

# Configure the TestConfiguration.h file
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/TestConfiguration.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/TestConfiguration.h
  @ONLY )

# To find TestConfiguration.h
include_directories(BEFORE ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(ImageTests)
add_subdirectory(OptimizeTests)
add_subdirectory(PythonTests)
add_subdirectory(ParticlesTests)
