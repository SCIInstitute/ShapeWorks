# NOTE: Do not use this cmake directly. Instead, use Superbuild.cmake

cmake_minimum_required(VERSION 3.1)  # this is minimum cmake version for some of our external libs (e.g., libigl)
set(CMAKE_CXX_STANDARD 11)           # available options are [98, 11, 14, 17. 20]

# Build options
option(USE_OPENMP      "Build using OpenMP" ON)
option(Build_PrepTools "Build prep tools"   ON)
option(Build_Run       "Build shapeworks"   ON)
option(Build_Post      "Build post"         ON)
option(Build_View2     "Build view2"        OFF)
option(Build_Studio    "Build studio"       OFF)

### Configuration
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/CMake)
set(SHAPEWORKS_ROOT     "${CMAKE_CURRENT_SOURCE_DIR}")
set(SHAPEWORKS_EXTERNAL "${SHAPEWORKS_ROOT}/ExternalLibs")


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

FIND_PACKAGE(ITK REQUIRED)
IF (ITK_USE_FILE)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_USE_FILE)
  MESSAGE(SEND_ERROR "ITK (The Insight Toolkit) is required, but could not be found.")
ENDIF(ITK_USE_FILE)

FIND_PACKAGE(VXL)
IF (VXL_FOUND)
  INCLUDE (${VXL_CMAKE_DIR}/UseVXL.cmake)
ELSE(VXL_FOUND)
  MESSAGE(SEND_ERROR "VXL is required, but could not be found.")
ENDIF (VXL_FOUND)

# Find OpenMP
if(APPLE AND USE_OPENMP)
  if(CMAKE_C_COMPILER_ID MATCHES "Clang")
    set(OpenMP_C "${CMAKE_C_COMPILER}")
    set(OpenMP_C_FLAGS "-fopenmp=libomp -Wno-unused-command-line-argument")
    set(OpenMP_C_LIB_NAMES "libomp" "libgomp" "libiomp5")
    set(OpenMP_libomp_LIBRARY ${OpenMP_C_LIB_NAMES})
    set(OpenMP_libgomp_LIBRARY ${OpenMP_C_LIB_NAMES})
    set(OpenMP_libiomp5_LIBRARY ${OpenMP_C_LIB_NAMES})
  endif()
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(OpenMP_CXX "${CMAKE_CXX_COMPILER}")
    set(OpenMP_CXX_FLAGS "-fopenmp=libomp -Wno-unused-command-line-argument")
    set(OpenMP_CXX_LIB_NAMES "libomp" "libgomp" "libiomp5")
    set(OpenMP_libomp_LIBRARY ${OpenMP_CXX_LIB_NAMES})
    set(OpenMP_libgomp_LIBRARY ${OpenMP_CXX_LIB_NAMES})
    set(OpenMP_libiomp5_LIBRARY ${OpenMP_CXX_LIB_NAMES})
  endif()
endif()

if(USE_OPENMP)
  find_package(OpenMP REQUIRED)
  add_definitions(-DSW_USE_OPENMP)
  add_definitions(-DMP_USE_OPENMP)
  if(APPLE)
    # the find_package flags just don't work for the latest clang, and I'm also not sure how to find the include path.
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Xpreprocessor -fopenmp -lomp -Wno-unused-command-line-argument -I\"/Users/cam/tools/miniconda3/envs/shapeworks_build-000/include\" -L\"/Users/cam/tools/miniconda3/envs/shapeworks_build-000/lib\"")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xpreprocessor -fopenmp -lomp -Wno-unused-command-line-argument -I\"/Users/cam/tools/miniconda3/envs/shapeworks_build-000/include\" -L\"/Users/cam/tools/miniconda3/envs/shapeworks_build-000/lib\"")
  else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")  #these don't exist, see cmake FindOpenMP (https://github.com/Kitware/CMake/blob/master/Modules/FindOpenMP.cmake)
  endif(APPLE)
endif(USE_OPENMP)

if(APPLE)
    set(CMAKE_INSTALL_RPATH "@executable_path/../lib")
else()
    set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
endif()

add_subdirectory(${SHAPEWORKS_EXTERNAL})
add_subdirectory(Libs)

#todo: setup INSTALL paths as described in Trello (otherwise 'make install' just puts everything in the root path of the specified installation directory)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
MESSAGE(STATUS "** USE_VTK_FILE: ${VTK_USE_FILE}")

include_directories(${SHAPEWORKS_ROOT})

if(Build_PrepTools)
  add_subdirectory(Prep/Source)
endif(Build_PrepTools)
if(Build_Run)
  add_subdirectory(Run/source)
endif(Build_Run)
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


set(APP ${CMAKE_INSTALL_PREFIX}/stl2vtk) 

# set paths to dependencies
set(DIRS ${CMAKE_INSTALL_PREFIX}/lib )

foreach(plugin ${Qt5Network_PLUGINS})
  get_target_property(_loc ${plugin} LOCATION)
  message("Plugin ${plugin} is at location ${_loc}")
endforeach()

get_filename_component(Qt_BIN_DIR "${QT_QMAKE_EXECUTABLE}" PATH)
get_filename_component(Qt_PREFIX_DIR "${Qt_BIN_DIR}" PATH)
get_filename_component(QT_LOC Qt5::Widgets DIRECTORY)
MESSAGE(STATUS "QT libs: ${QT_LOC}")

set(qtlib "Widgets")
MESSAGE(STATUS "QT libs: ${QT_LIB_DIR_tmp}")

get_filename_component(qt_dlls_dir "${QtWidgets_location}" PATH)
get_filename_component(qt_dlls_dir "${qt_dlls_dir}" PATH)

get_target_property(QT5_WIDGETSLIBRARY Qt5::Widgets LOCATION)
get_filename_component(QT5_WIDGETSLIBRARYPATH ${QT5_WIDGETSLIBRARY} PATH)
MESSAGE(STATUS "QT widget: ${QT5_WIDGETSLIBRARYPATH}")
set(DIRS "${QT5_WIDGETSLIBRARYPATH} \${CMAKE_INSTALL_PREFIX}/lib")

set(DIRS "${CMAKE_INSTALL_PREFIX}/lib")

list(APPEND DIRS "${QT5_WIDGETSLIBRARYPATH}") 

# <ctc> try without this to see if it's actually necessary
#include(InstallRequiredSystemLibraries)

MESSAGE(STATUS "CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}")


