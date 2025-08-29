cmake_minimum_required(VERSION 3.20)

##########################################
# SHAPEWORKS VERSION
###########################################
SET(SHAPEWORKS_MAJOR_VERSION 6 CACHE INTERNAL "Major version number" FORCE)
SET(SHAPEWORKS_MINOR_VERSION 7 CACHE INTERNAL "Minor version number" FORCE)
SET(SHAPEWORKS_PATCH_VERSION 0 CACHE INTERNAL "Patch version number" FORCE)
SET(SHAPEWORKS_VERSION_STRING "6.7.0-dev")
SET(SHAPEWORKS_VERSION "${SHAPEWORKS_MAJOR_VERSION}.${SHAPEWORKS_MINOR_VERSION}.${SHAPEWORKS_PATCH_VERSION}")

# First, check that files were checked out properly using git-lfs
file(MD5 "${CMAKE_CURRENT_SOURCE_DIR}/Testing/data/icp_baseline.nrrd" HASH)
if (NOT "${HASH}" STREQUAL "bb94438a695c749b264180019abbbb97")
  message( FATAL_ERROR "MD5 hash of '${CMAKE_CURRENT_SOURCE_DIR}/Testing/data/icp_baseline.nrrd' is incorrect.  This most likely means that git-lfs was not installed when ShapeWorks was cloned.  If you have downloaded a zip or tar.gz of the source, then it will be missing Git-LFS objects.  Please use git to clone ShapeWorks or download a source archive containing the Git-LFS objects under releases." )
endif()

set(CMAKE_CXX_STANDARD 17)             # available options are [98, 11, 14, 17. 20]

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
find_package(Python3 3.9 EXACT REQUIRED COMPONENTS Interpreter Development)

if (NOT APPLE)
  option(USE_OPENMP      "Build using OpenMP" ON)
endif()
option(BUILD_TESTS     "Build tests"        ON)

option(USE_PCH "Use pre-compiled headers" OFF)

option(USE_ORIGIN_RPATH "Use $ORIGIN in rpath, for deployment" OFF)

SET(GA_MEASUREMENT_ID "" CACHE STRING "Google Analytics Measurement ID")
SET(GA_API_SECRET "" CACHE STRING "Google Analytics API Secret")

set(SHAPEWORKS_QT_REQUIRED REQUIRED)

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/install" CACHE STRING "Install path prefix, prepended onto install directories.")

include(DefaultBuildType)

project(ShapeWorks)


### vtk
find_package(VTK 9.5)
if (NOT VTK_FOUND)
  message(FATAL_ERROR "VTK is required, but could not be found! Make sure path to VTKConfig.cmake is included in CMAKE_PREFIX_PATH")
endif()

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
  SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--disable-new-dtags ${CMAKE_EXE_LINKER_FLAGS}")
  SET(CMAKE_SHARED_LINKER_FLAGS "-Wl,--disable-new-dtags")
endif(LINUX)

message(STATUS "CMAKE_SHARED_LINKER_FLAGS: ${CMAKE_SHARED_LINKER_FLAGS}")

# put executables in bin directory (under build dir) so they are easy to find
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_BUNDLE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (APPLE)
  # make it easier to see more important compiler warnings and errors (# TODO: fix the actual cause of the warnings)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-inconsistent-missing-override")

  # workaround for version of boost in use
  add_definitions(-D_LIBCPP_ENABLE_CXX17_REMOVED_UNARY_BINARY_FUNCTION)
endif()

if(MSVC)
  # turn on unwind semantics
  SET(CMAKE_CXX_FLAGS "/EHsc")
endif(MSVC)

# GUI
find_package(Qt5 COMPONENTS Core Widgets OpenGL Gui Sql Xml Svg PrintSupport ${SHAPEWORKS_QT_REQUIRED})
if (Qt5Core_FOUND AND Qt5Widgets_FOUND AND Qt5OpenGL_FOUND AND Qt5Gui_FOUND AND Qt5Sql_FOUND)
  set(SHAPEWORKS_QT_FOUND 1)
  if (Qt5Widgets_VERSION VERSION_LESS 5.15.4)
      message(FATAL_ERROR "Minimum supported Qt5 version is 5.15.4!  Add the shapeworks conda env's bin dir to the start of $PATH")
  endif()
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

add_definitions(-DQT_NO_KEYWORDS)
  
if (WIN32)
  cmake_policy(SET CMP0020 NEW)   # link to qtmain.lib on Windows
else()
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-write-strings")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
endif()

### Eigen
find_package (Eigen3 3.3 REQUIRED NO_MODULE)

### ITK
FIND_PACKAGE(ITK 5.3.0 REQUIRED)
IF (ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE()
  MESSAGE(SEND_ERROR "ITK (The Insight Toolkit) is required, but could not be found! Make sure path to ITKConfig.cmake is included in CMAKE_PREFIX_PATH")
ENDIF()

### XLNT
find_package (XLNT REQUIRED)
add_definitions(-DXLNT_STATIC=1)

### ACVD
find_package (ACVD REQUIRED)

### Libigl set include path
find_package(LIBIGL REQUIRED QUIET)

### Boost
find_package(Boost REQUIRED COMPONENTS
             filesystem)

### Json library
find_package(nlohmann_json 3.10.5 REQUIRED)

### spdlog library
find_package(spdlog 1.10.0 REQUIRED)

### Geometry Central
find_package (GEOMETRYCENTRAL REQUIRED)

if(USE_OPENMP)
  find_package(OpenMP REQUIRED)
  add_definitions(-DSW_USE_OPENMP) # used in ExternalLibs/trimesh2/include/TriMesh.h
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -fPIC")
endif(USE_OPENMP)

SET(CMAKE_INSTALL_LIBDIR lib)

if(APPLE)
  set(CMAKE_INSTALL_RPATH "@executable_path/ShapeWorksStudio.app/Contents/Frameworks")
else()
  if(USE_ORIGIN_RPATH)
    set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
  else()
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
  endif()
    
  set(CMAKE_SKIP_BUILD_RPATH FALSE)
  set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Libs")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Libs/Common")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ExternalLibs/optparse")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ExternalLibs/robin_hood")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ExternalLibs/ordered_map")

FILE(GLOB ORDERED_MAP_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/ExternalLibs/ordered_map/*.h")
INSTALL(FILES ${ORDERED_MAP_INCLUDES}
  DESTINATION include)

SET(PYBIND11_PYTHON_VERSION 3.9)
find_package(pybind11 2.5.0 REQUIRED)

add_subdirectory(ExternalLibs)
add_subdirectory(Libs)
add_subdirectory(Applications)
add_subdirectory(Studio)

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

  add_subdirectory(docs/doxygen)
endif(BUILD_DOCUMENTATION)

MESSAGE(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
MESSAGE(STATUS "CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}")
message(STATUS "CMAKE_SHARED_LINKER_FLAGS: ${CMAKE_SHARED_LINKER_FLAGS}")

############################################################
# Below to handle installation and use as external library
############################################################

set(SHAPEWORKS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SHAPEWORKS_LIBS Project)
set(SHAPEWORKS_CMAKE_CFG_DEST_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

install(TARGETS igl igl_common EXPORT ShapeWorksTargets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION include
  )

install(EXPORT ShapeWorksTargets
  FILE ShapeWorksTargets.cmake
  NAMESPACE shapeworks::
  DESTINATION ${SHAPEWORKS_CMAKE_CFG_DEST_DIR})

include(CMakePackageConfigHelpers)

configure_package_config_file(cmake/ShapeWorksConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/ShapeWorksConfig.cmake
  INSTALL_DESTINATION ${SHAPEWORKS_CMAKE_CFG_DEST_DIR}
  PATH_VARS SHAPEWORKS_INCLUDE_DIR)

write_basic_package_version_file(ShapeWorksConfigVersion.cmake
  VERSION ${SHAPEWORKS_VERSION}
  COMPATIBILITY ExactVersion)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/ShapeWorksConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/ShapeWorksConfigVersion.cmake
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindXLNT.cmake
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindLIBIGL.cmake
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindACVD.cmake
  DESTINATION ${SHAPEWORKS_CMAKE_CFG_DEST_DIR})
