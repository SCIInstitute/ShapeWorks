cmake_minimum_required(VERSION 3.11)
set(CMAKE_CXX_STANDARD 14)             # available options are [98, 11, 14, 17. 20]
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake")
find_package(Python3 3.7 EXACT REQUIRED)


##########################################
# SHAPEWORKS VERSION
###########################################
SET(SHAPEWORKS_MAJOR_VERSION 6 CACHE INTERNAL "Major version number" FORCE)
SET(SHAPEWORKS_MINOR_VERSION 2 CACHE INTERNAL "Minor version number" FORCE)
SET(SHAPEWORKS_PATCH_VERSION 0 CACHE INTERNAL "Patch version number" FORCE)
SET(SHAPEWORKS_VERSION_STRING "6.2.0-rc5")


#
# ShapeWorks options
#
if (NOT APPLE)
  option(USE_OPENMP      "Build using OpenMP" ON)
endif()
option(Build_Studio    "Build studio"       OFF)
option(BUILD_TESTS     "Build tests"        ON)

if ("${Build_Studio}")
  set(SHAPEWORKS_GUI ON)
  set(SHAPEWORKS_QT_REQUIRED REQUIRED)
else()
  set(SHAPEWORKS_GUI OFF)
endif()
message(STATUS "SHAPEWORKS_GUI: ${SHAPEWORKS_GUI}")

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/install" CACHE STRING "Install path prefix, prepended onto install directories.")

include(DefaultBuildType)

project(ShapeWorks)

# OpenVDB only provides a module file (FindOpenVDB.cmake), so we use OpenVDB_DIR to find it
if (NOT DEFINED OpenVDB_DIR OR "${OpenVDB_DIR}" STREQUAL "OpenVDB_DIR-NOTFOUND")
  set(OpenVDB_DIR ${CMAKE_PREFIX_PATH}/lib/cmake/OpenVDB)
endif()
message(STATUS "OpenVDB_DIR set to ${OpenVDB_DIR} (must contain FindOpenVDB.cmake)")
file(TO_CMAKE_PATH ${OpenVDB_DIR} OpenVDB_DIR) # fixes path to use forward slashes on Windows
list(APPEND CMAKE_MODULE_PATH ${OpenVDB_DIR})
find_package(OpenVDB MODULE REQUIRED)

# use ccache if available
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

if(UNIX AND NOT APPLE)
  set(LINUX TRUE)
endif()

if(LINUX)
  # We need RPATH rather than RUNPATH since RUNPATH
  # though recommended, is not used for transitive dependencies
  SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--disable-new-dtags")
  SET(CMAKE_SHARED_LINKER_FLAGS "-Wl,--disable-new-dtags")
endif(LINUX)

message(STATUS "CMAKE_SHARED_LINKER_FLAGS: ${CMAKE_SHARED_LINKER_FLAGS}")

# put executables in bin directory (under build dir) so they are easy to find
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_BUNDLE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# make it easier to see more important compiler warnings and errors (# TODO: fix the actual cause of the warnings)
if (APPLE)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-inconsistent-missing-override")
endif()

if(MSVC)
  # turn on unwind semantics
  SET(CMAKE_CXX_FLAGS "/EHsc")
endif(MSVC)

# GUI
if (SHAPEWORKS_GUI)
    
  find_package(Qt5 COMPONENTS Core Widgets OpenGL Gui Sql Xml Svg PrintSupport ${SHAPEWORKS_QT_REQUIRED})
  if (Qt5Core_FOUND AND Qt5Widgets_FOUND AND Qt5OpenGL_FOUND AND Qt5Gui_FOUND AND Qt5Sql_FOUND)
  set(SHAPEWORKS_QT_FOUND 1)
  else()
  set(SHAPEWORKS_QT_FOUND 0)
  endif()

  find_package(JKQTCommonSharedLib REQUIRED)
  find_package(JKQTMathTextSharedLib REQUIRED)
  find_package(JKQTPlotterSharedLib REQUIRED)

  # headers (TODO: move to shapeworks gui targets' includes)
  include_directories(${Qt5Widgets_INCLUDE_DIRS})
  include_directories(${Qt5OpenGL_INCLUDE_DIRS})
  include_directories(${Qt5Core_INCLUDE_DIRS})
  include_directories(${Qt5Gui_INCLUDE_DIRS})
  include_directories(${Qt5Sql_INCLUDE_DIRS})
  
  # defines (TODO: move to shapeworks gui targets' includes)
  add_definitions(${Qt5Widgets_DEFINITIONS})
  add_definitions(${Qt5OpenGL_DEFINITIONS})
  add_definitions(${Qt5Core_DEFINITIONS})
  add_definitions(${Qt5Gui_DEFINITIONS})
  add_definitions(${Qt5Sql_DEFINITIONS})
  
  if (WIN32)
    cmake_policy(SET CMP0020 NEW)   # link to qtmain.lib on Windows
  else()
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-write-strings")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
  endif()
endif()

### vtk
find_package(VTK ${SHAPEWORKS_VTK_VERSION_MIN})
if (NOT VTK_FOUND)
  message(FATAL_ERROR "VTK is required, but could not be found! Make sure path to VTKConfig.cmake is included in CMAKE_PREFIX_PATH")
endif()
include(${VTK_USE_FILE})

### vxl
FIND_PACKAGE(VXL REQUIRED)
IF(VXL_FOUND)
  # this adds deprecated VXL variables to the cmake namespace (TODO: use the new ones)
  INCLUDE (${VXL_CMAKE_DIR}/UseVXL.cmake)
ELSE(VXL_FOUND)
  MESSAGE(SEND_ERROR "VXL is required, but could not be found! Make sure path to VXLConfig.cmake is included in CMAKE_PREFIX_PATH")
ENDIF(VXL_FOUND)

### Eigen
find_package (Eigen3 3.3 REQUIRED NO_MODULE)

### ITK
FIND_PACKAGE(ITK ${SHAPEWORKS_ITK_VERSION_MIN} REQUIRED)
IF (ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE()
  MESSAGE(SEND_ERROR "ITK (The Insight Toolkit) is required, but could not be found! Make sure path to ITKConfig.cmake is included in CMAKE_PREFIX_PATH")
ENDIF()

### XLNT
find_package (XLNT REQUIRED)
add_definitions(-DXLNT_STATIC=1)

### Libigl set include path
find_package(LIBIGL REQUIRED QUIET)

find_package(Boost REQUIRED COMPONENTS
             filesystem)

### Geometry central include
if (NOT DEFINED GEOMETRY_CENTRAL_DIR)
  set(GEOMETRY_CENTRAL_DIR ${CMAKE_PREFIX_PATH}/geometry-central/)
endif()
# we need to explicitly specify the output folder if adding a subdirectory outside the current working dir.
add_subdirectory(${GEOMETRY_CENTRAL_DIR} ./geometry-central-build/)

if(USE_OPENMP)
  find_package(OpenMP REQUIRED)
  add_definitions(-DSW_USE_OPENMP) # used in ExternalLibs/trimesh2/include/TriMesh.h
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -fPIC")
endif(USE_OPENMP)

if(APPLE)
  set(CMAKE_INSTALL_RPATH "@executable_path/../lib")
else()
  set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
endif()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Libs/Common")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ExternalLibs/optparse")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ExternalLibs/robin_hood")

SET(PYBIND11_PYTHON_VERSION 3.7)
find_package(pybind11 2.5.0 REQUIRED)

add_subdirectory(ExternalLibs)
add_subdirectory(Libs)
add_subdirectory(Applications)

if(Build_Studio)
  add_subdirectory(Studio/src)
endif(Build_Studio)

if (SHAPEWORKS_GUI)
  foreach(plugin ${Qt5Network_PLUGINS})
    get_target_property(_loc ${plugin} LOCATION)
    message("Plugin ${plugin} is at location ${_loc}")
  endforeach()
  
  get_filename_component(Qt_BIN_DIR "${QT_QMAKE_EXECUTABLE}" PATH)
  get_filename_component(Qt_PREFIX_DIR "${Qt_BIN_DIR}" PATH)
  get_filename_component(QT_LOC Qt5::Widgets DIRECTORY)
  
  set(qtlib "Widgets")
  
  get_filename_component(qt_dlls_dir "${QtWidgets_location}" PATH)
  get_filename_component(qt_dlls_dir "${qt_dlls_dir}" PATH)
  
  get_target_property(QT5_WIDGETSLIBRARY Qt5::Widgets LOCATION)
  get_filename_component(QT5_WIDGETSLIBRARYPATH ${QT5_WIDGETSLIBRARY} PATH)
  MESSAGE(STATUS "QT widget: ${QT5_WIDGETSLIBRARYPATH}")
  set(DIRS "${CMAKE_INSTALL_PREFIX}/lib")
  list(APPEND DIRS "${QT5_WIDGETSLIBRARYPATH}")
endif()

#----------------------------------------------------------------------------
if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(Testing)
endif(BUILD_TESTS)

#----------------------------------------------------------------------------
# Add an option to toggle the generation of the API documentation
option(BUILD_DOCUMENTATION "Use Doxygen to create the HTML based API documentation" OFF)
if(BUILD_DOCUMENTATION)

  # for doxygen
  SET(ShapeWorks_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
  SET(ShapeWorks_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

  find_package(Doxygen)
  if(NOT DOXYGEN_FOUND)
    message(FATAL_ERROR
      "Doxygen is needed to build the documentation. Please install it correctly")
  endif(NOT DOXYGEN_FOUND)

  add_subdirectory(docs/Doxygen)
endif(BUILD_DOCUMENTATION)

MESSAGE(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
MESSAGE(STATUS "CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}")
message(STATUS "CMAKE_SHARED_LINKER_FLAGS: ${CMAKE_SHARED_LINKER_FLAGS}")

