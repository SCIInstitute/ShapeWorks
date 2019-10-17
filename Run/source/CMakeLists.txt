PROJECT(SHAPEWORKS)
cmake_minimum_required(VERSION 2.6)

#SET(ITK_DIR "path-to-ITK-4.7.2-build")
#SET(VTK_DIR "path-to-VTK-5.10.1-build")
#SET(VXL_DIR "path-to-vxl-build")
SET(CMAKE_BUILD_TYPE Release)

#Match everything when analyzing dependencies
INCLUDE_REGULAR_EXPRESSION("^.*$")

# Set up the include directories
#include_directories ( 
#${SHAPEWORKS_SOURCE_DIR}/Utilities
#)

# Command-line tools are built by default.
subdirs(ShapeWorksRun)

IF (WIN32 AND MSVC)
  ADD_DEFINITIONS(-D_BIND_TO_CURRENT_VCLIBS_VERSION=1)
ENDIF (WIN32 AND MSVC)

