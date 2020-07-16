
# GTest setup
find_package(GTest REQUIRED)
# NOTE: must call enable_testing() before this in the top-level CMakeLists.txt

# Set the test data location
set(TEST_DATA_DIR "${CMAKE_SOURCE_DIR}/Testing/data")

# Configure the TestConfiguration.h file
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/TestConfiguration.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/TestConfiguration.h
  @ONLY )

# To find TestConfiguration.h
include_directories(BEFORE ${CMAKE_CURRENT_BINARY_DIR})




# Shared Testing library
set(Testing_sources
  Testing.cpp
  )
set(Testing_headers
  Testing.h
  )
add_library(Testing STATIC
  ${Testing_sources}
  ${Testing_headers})
target_include_directories(Testing PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>)
target_link_libraries(Testing
  GTest::Main
  )
set_target_properties(Testing PROPERTIES PUBLIC_HEADER
  "${Testing_headers}")
install(TARGETS Testing
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION lib
  LIBRARY DESTINATION lib
  PUBLIC_HEADER DESTINATION include
  )




# Individual library tests
add_subdirectory(ImageTests)
add_subdirectory(MeshTests)
add_subdirectory(OptimizeTests)
add_subdirectory(PythonTests)
add_subdirectory(ParticlesTests)
add_subdirectory(shapeworksTests)
