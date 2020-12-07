# - Try to find the LIBIGL library
# Once done this will define
#
#  LIBIGL_FOUND - system has LIBIGL
#  LIBIGL_INCLUDE_DIR - **the** LIBIGL include directory
if(LIBIGL_FOUND)
    return()
endif()

set(LIBIGL_DIR "$ENV{LIBIGL_DIR}" CACHE PATH "LIBIGL root directory.")
message("Looking for LIBIGL in ${LIBIGL_DIR}")

find_path(LIBIGL_INCLUDE_DIR
  NAMES libigl/include/igl/readOBJ.h
  HINTS ${CMAKE_SOURCE_DIR}
  HINTS ${LIBIGL_DIR}
)

set(LIBIGL_INCLUDE_DIR ${LIBIGL_INCLUDE_DIR}/libigl/cmake)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LIBIGL
    "\nlibigl not found --- You can download it using:\n\tgit clone https://github.com/libigl/libigl.git ${CMAKE_SOURCE_DIR}/../libigl"
    LIBIGL_INCLUDE_DIR)
mark_as_advanced(LIBIGL_INCLUDE_DIR)

if(LIBIGL_FOUND)
  message("-- Found LIBIGL under ${LIBIGL_INCLUDE_DIR}")
endif(LIBIGL_FOUND)

list(APPEND CMAKE_MODULE_PATH "${LIBIGL_INCLUDE_DIR}/../cmake")
include(libigl)
