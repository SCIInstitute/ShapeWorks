# GTest setup
#find_package(GTest REQUIRED)

# fetch googletest
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.11.0
)
FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
  FetchContent_Populate(googletest)
  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
endif()


# NOTE: must call enable_testing() before this in the top-level CMakeLists.txt

# Set the test data location
set(TEST_DATA_DIR "${CMAKE_SOURCE_DIR}/Testing/data")

# Set the Python modules location
set(PYTHON_EXAMPLES_DIR "${CMAKE_SOURCE_DIR}/Examples/Python")

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
  gtest_main
  ${Boost_LIBRARIES}
  )
set_target_properties(Testing PROPERTIES PUBLIC_HEADER
  "${Testing_headers}")
install(TARGETS Testing
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION include
  )

# Individual library tests
add_subdirectory(ImageTests)
add_subdirectory(MeshTests)
add_subdirectory(GroomTests)
add_subdirectory(OptimizeTests)
add_subdirectory(PythonTests)
add_subdirectory(ParticlesTests)
add_subdirectory(ProjectTests)
add_subdirectory(UseCaseTests)
add_subdirectory(shapeworksTests)
add_subdirectory(UtilsTests)
