project(ITKParticleShapeModeling)
cmake_minimum_required(VERSION 2.6)

# Compile with c++11 functionality.
if(UNIX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=gnu++11") # gcc >= 4.7
endif(UNIX)

file(
  GLOB
  source_files
  src/*.txt
  src/*.cxx
  include/*.txt
  include/*.h
  include/*.hxx
  )
OPTION(PSM_BUILD_EXAMPLES "Build PSM Examples" OFF)
IF (PSM_BUILD_EXAMPLES)
  add_subdirectory(Examples)
ENDIF (PSM_BUILD_EXAMPLES)
OPTION(PSM_BUILD_TESTS "Build PSM Tests" OFF)
IF (PSM_BUILD_TESTS)
  add_subdirectory(test)
ENDIF (PSM_BUILD_TESTS)

INCLUDE_DIRECTORIES(include src)

#create a library to link to
ADD_LIBRARY(itkParticleShapeModeling ${source_files})
TARGET_LINK_LIBRARIES(itkParticleShapeModeling ${ITK_LIBRARIES})
