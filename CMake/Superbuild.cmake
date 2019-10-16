# ShapeWorks Superbuild
#  Includes external libraries automatically which are built at compilation time rather than during configuration.

project(ShapeWorksSuperbuild)
include(ExternalProject)

# CMake arguments common to all the external projects
set(ep_common_args
  -DCMAKE_BUILD_TYPE:STRING=${SHAPEWORKS_EXTERNALS_CMAKE_BUILD_TYPE}
  -DCMAKE_INSTALL_PREFIX:STRING=${CMAKE_INSTALL_PREFIX}
  -DCMAKE_CXX_COMPILER:STRING=${CMAKE_CXX_COMPILER}
  -DCMAKE_C_COMPILER:STRING=${CMAKE_C_COMPILER}
  -DCMAKE_GENERATOR:STRING=${CMAKE_GENERATOR}
  -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${CMAKE_BINARY_DIR}/bin
  -DCMAKE_CXX_FLAGS=""
  )

# Compute -G arg for configuring external projects with the same CMake generator:
if(CMAKE_EXTRA_GENERATOR)
  set(gen "${CMAKE_EXTRA_GENERATOR} - ${CMAKE_GENERATOR}")
else()
  set(gen "${CMAKE_GENERATOR}" )
endif()

set(ShapeWorks_DEPENDENCIES )

if(NOT VTK_DIR)
  include("${CMAKE_CURRENT_LIST_DIR}/External-VTK.cmake")
  list(APPEND ShapeWorks_DEPENDENCIES vtk)
endif()

if(NOT VXL_DIR)
  include("${CMAKE_CURRENT_LIST_DIR}/External-VXL.cmake")
  list(APPEND ShapeWorks_DEPENDENCIES vxl)
endif()

if(NOT ITK_DIR)
  include("${CMAKE_CURRENT_LIST_DIR}/External-ITK.cmake")
  list(APPEND ShapeWorks_DEPENDENCIES ITK)
endif()
