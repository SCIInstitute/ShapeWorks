PROJECT(SHAPEWORKS)
cmake_minimum_required(VERSION 2.6)

#SET(ITK_DIR "Path-to-ITK-4.7.2-build")
#SET(VTK_DIR "Path-to-VTK-5.10.1-build")
#SET(VXL_DIR "Path-to-vxl-build")
SET(CMAKE_BUILD_TYPE RELEASE)

SET(BUILD_EXECS_AT_SAME_LEVEL FALSE CACHE BOOL "For use with ShapeWorksRemote, build all executable outputs in the same directory." )
IF (BUILD_EXECS_AT_SAME_LEVEL)
  SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/binary)
  SET(CMAKE_BUNDLE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/binary)
ENDIF (BUILD_EXECS_AT_SAME_LEVEL)

#Match everything when analyzing dependencies
INCLUDE_REGULAR_EXPRESSION("^.*$")

# Configure ITK -- REQUIRED
find_package(ITK REQUIRED)
if(ITK_FOUND)
  include(${ITK_USE_FILE})
else(ITK_FOUND)
  message(SEND_ERROR "ITK (The Insight Toolkit) is required, but could not be found.")
endif(ITK_FOUND)

# Configure VTK -- REQUIRED
FIND_PACKAGE(VTK REQUIRED)
IF (VTK_FOUND)
INCLUDE (${VTK_USE_FILE})
ELSE(VTK_FOUND)
MESSAGE(FATAL_ERROR "VTK (The Visualization Toolkit) is required for ShapeWorksView and ShapeWorksShop, but could not be found.")
ENDIF (VTK_FOUND)

# Optionally build with mesh support (requires linking against trimesh library)
option(BUILD_MeshSupport "Build with Mesh support (requires fim library with trimesh2)" OFF)

# Fea mesh support
option(BUILD_FeaMeshSupport "Build with Fea Mesh support (requires fim library with trimesh2)" ON)

# Optionally use OpenMP
option(USE_OPENMP "Build parallel optimization using OpenMP" ON)
if(USE_OPENMP)
  FIND_PACKAGE( OpenMP REQUIRED)
  if(OPENMP_FOUND)
    message("Found OpenMP")
    add_definitions(-DSW_USE_OPENMP)
    add_definitions(-DMP_USE_OPENMP)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
  endif()
endif(USE_OPENMP)

# Configure Mesh support
if(BUILD_MeshSupport OR BUILD_FeaMeshSupport)
  SUBDIRS(fim_v4)
    INCLUDE_DIRECTORIES(fim_v4)
  INCLUDE_DIRECTORIES(fim_v4/trimesh2/include)
  if(BUILD_MeshSupport)
    add_definitions(-DSW_USE_MESH)
  endif(BUILD_MeshSupport)
  if(BUILD_FeaMeshSupport)
    add_definitions(-DSW_USE_FEAMESH)
  endif(BUILD_FeaMeshSupport)
  set(USE_VXL 1)
endif(BUILD_MeshSupport OR BUILD_FeaMeshSupport)

# Set up the include directories
include_directories ( 
${SHAPEWORKS_SOURCE_DIR}/Utilities
${SHAPEWORKS_SOURCE_DIR}/ITKParticleSystem 
${SHAPEWORKS_SOURCE_DIR}/tinyxml
)

# Command-line tools are built by default.
subdirs(tinyxml ITKParticleSystem Utilities ShapeWorksRun)

IF(USE_VXL)
FIND_PACKAGE(VXL)
IF (VXL_FOUND)
  INCLUDE (${VXL_DIR}/UseVXL.cmake)
ELSE(VTK_FOUND)
  MESSAGE(SEND_ERROR "VXL is required, but could not be found.")
ENDIF (VXL_FOUND)
ENDIF(USE_VXL)

IF (WIN32 AND MSVC)
  ADD_DEFINITIONS(-D_BIND_TO_CURRENT_VCLIBS_VERSION=1)
ENDIF (WIN32 AND MSVC)

