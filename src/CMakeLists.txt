PROJECT(SHAPEWORKS)
cmake_minimum_required(VERSION 2.6)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_BUNDLE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#Match everything when analyzing dependencies
INCLUDE_REGULAR_EXPRESSION("^.*$")

# Configure ITK -- REQUIRED
find_package(ITK REQUIRED)
if(ITK_FOUND)
  include(${ITK_USE_FILE})
else(ITK_FOUND)
  message(SEND_ERROR "ITK (The Insight Toolkit) is required, but could not be found.")
endif(ITK_FOUND)

# Optionally build programs.
option(BUILD_ShapeWorksView2 "Build ShapeWorksView2 (requires QT & VTK)" ON)
option(BUILD_Unsupported "Build unsupported tools (NOT recommended, requires FLTK & VTK)" OFF)

# Optionally use OpenMP
option(USE_OPENMP "Build parallel optimization using OpenMP" OFF)
if(USE_OPENMP)
  FIND_PACKAGE( OpenMP REQUIRED)
  if(OPENMP_FOUND)
    message("Found OpenMP")
    add_definitions(-DSW_USE_OPENMP)
    #    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
  endif()
endif(USE_OPENMP)

# Set up the include directories
include_directories (
  ${SHAPEWORKS_SOURCE_DIR}/ITKParticleSystem
  ${SHAPEWORKS_SOURCE_DIR}/tinyxml
  ${SHAPEWORKS_SOURCE_DIR}/Utilities
  )

# Command-line tools are built by default.
subdirs(tinyxml ITKParticleSystem Utilities ShapeWorksRun ShapeWorksGroom)

if (BUILD_ShapeWorksView2)
  set(USE_QT 1)
  set(USE_VTK 1)
endif(BUILD_ShapeWorksView2)

# Optionally configure VTK
IF(USE_VTK)
  FIND_PACKAGE(VTK COMPONENTS
    vtkCommonCore
    vtkIOCore
    vtkIOLegacy
    vtkIOGeometry
    vtkInfovisCore
    vtkInteractionStyle
    vtkRenderingFreeTypeOpenGL
    vtkViewsQt
    vtkFiltersCore
    vtkGUISupportQt
    vtkRenderingQt
    REQUIRED)
  IF (VTK_FOUND)
    INCLUDE (${VTK_USE_FILE})
  ELSE(VTK_FOUND)
    MESSAGE(FATAL_ERROR "VTK (required for ShapeWorksView2) could not be found.")
  ENDIF (VTK_FOUND)
ENDIF(USE_VTK)

IF(USE_QT)
  IF (WIN32)
    SET (QT_USE_QTMAIN TRUE)
  ENDIF (WIN32)
  SET(QT_MIN_VERSION "4.7.4")
  FIND_PACKAGE( Qt4 REQUIRED )
  INCLUDE( ${QT_USE_FILE} )
ENDIF(USE_QT)

IF (VTK_FOUND)
  IF(BUILD_ShapeWorksView2)
    SUBDIRS(ShapeWorksView2)
  ENDIF(BUILD_ShapeWorksView2)
ENDIF(VTK_FOUND)

IF (VTK_FOUND)
  IF (BUILD_ICPRigid3DRegistration)
    SUBDIRS(ICPRigid3DRegistration)
  ENDIF(BUILD_ICPRigid3DRegistration)
ENDIF(VTK_FOUND)

IF (WIN32 AND MSVC)
  ADD_DEFINITIONS(-D_BIND_TO_CURRENT_VCLIBS_VERSION=1)
ENDIF (WIN32 AND MSVC)

if (BUILD_Unsupported)
  subdirs(deprecated)
endif()
