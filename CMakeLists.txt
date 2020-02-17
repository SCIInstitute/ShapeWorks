cmake_minimum_required(VERSION 3.11)
set(CMAKE_CXX_STANDARD 11)             # available options are [98, 11, 14, 17. 20]
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake")

#
# ShapeWorks options
#
if (NOT APPLE)
  option(USE_OPENMP      "Build using OpenMP" ON)
endif()
option(Build_Post      "Build post"         OFF)
option(Build_View2     "Build view2"        OFF)
option(Build_Studio    "Build studio"       OFF)
option(BUILD_TESTS     "Build tests"        ON)
if ("${Build_Post}" OR "${Build_View2}" OR "${Build_Studio}")
  set(SHAPEWORKS_GUI ON)
  set(SHAPEWORKS_QT_REQUIRED REQUIRED)
else()
  set(SHAPEWORKS_GUI OFF)
endif()
message(STATUS "SHAPEWORKS_GUI: ${SHAPEWORKS_GUI}")

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/install" CACHE STRING "Install path prefix, prepended onto install directories.")

include(DefaultBuildType)

# Google Test
if (BUILD_TESTS)
  message(STATUS "Building Google Test")
  set( gtest_force_shared_crt ON CACHE BOOL "Always use msvcrt.dll" FORCE)
  enable_testing()
  include(FetchContent)
  
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        release-1.10.0
    )
  
  FetchContent_GetProperties(googletest)
  if(NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
  endif()
endif(BUILD_TESTS)


project(ShapeWorks)

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
endif(LINUX)

# put executables in bin directory (under build dir) so they are easy to find
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_BUNDLE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# GUI
find_package(Qt5 COMPONENTS Core Widgets OpenGL Gui Sql ${SHAPEWORKS_QT_REQUIRED})
if (Qt5Core_FOUND AND Qt5Widgets_FOUND AND Qt5OpenGL_FOUND AND Qt5Gui_FOUND AND Qt5Sql_FOUND)
  set(SHAPEWORKS_QT_FOUND 1)
else()
  set(SHAPEWORKS_QT_FOUND 0)
endif()

if (SHAPEWORKS_GUI)
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

### ITK
FIND_PACKAGE(ITK ${SHAPEWORKS_ITK_VERSION_MIN} REQUIRED)
IF (ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE()
  MESSAGE(SEND_ERROR "ITK (The Insight Toolkit) is required, but could not be found! Make sure path to ITKConfig.cmake is included in CMAKE_PREFIX_PATH")
ENDIF()

### Eigen
find_package (Eigen3 3.3 REQUIRED NO_MODULE)

if(USE_OPENMP)
  find_package(OpenMP REQUIRED)
  add_definitions(-DSW_USE_OPENMP) # used in ExternalLibs/trimesh2/include/TriMesh.h
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif(USE_OPENMP)

if(APPLE)
  set(CMAKE_INSTALL_RPATH "@executable_path/../lib")
else()
  set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
endif()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Libs/Common")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ExternalLibs/optparse")

# For consolidating the independent executables. Remove when done (TODO)
add_definitions(-DDEBUG_CONSOLIDATION=0)

add_subdirectory(ExternalLibs)
add_subdirectory(Libs)
add_subdirectory(Applications)

if(Build_Post)
  add_subdirectory(Post/source/ShapeWorksPost-V1)
endif(Build_Post)
if(Build_View2)
  MESSAGE(STATUS "Building View2")
  add_subdirectory(Post/source/ShapeWorksView2)
endif(Build_View2)
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

if(BUILD_TESTS)
  add_subdirectory(Testing)
endif(BUILD_TESTS)

#--------------------------------------------------------------------------------
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

  add_subdirectory(Documentation/Doxygen)
endif(BUILD_DOCUMENTATION)

MESSAGE(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
MESSAGE(STATUS "CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}")
