PROJECT(SHAPEWORKS)
cmake_minimum_required(VERSION 2.6)

#---- Setting variables --------
#SET(ITK_DIR "/scratch/TOOLKITS/InsightToolkit-4.5.0-build")
#SET(VTK_DIR "/scratch/TOOLKITS/VTK5.10.1-BUILD")
#SET(FLTK_DIR "/scratch/TOOLKITS/fltk-1.1.10")

SET(ITK_DIR "/home/sci/prafulag/BuildDIR/ITK-4.7.2")
SET(VTK_DIR "/home/sci/prafulag/BuildDIR/VTK-5.10.1")
SET(FLTK_DIR "/home/sci/prafulag/BuildDIR/fltk-1.1.10")
SET(USE_VTK 1)
SET(CMAKE_BUILD_TYPE DEBUG)
#-------------------------------

SET(VALGRIND_COMPILE_FLAGS "-g -O1")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${VALGRIND_COMPILE_FLAGS}")

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

# Optionally build with mesh support (requires linking against trimesh library)
option(BUILD_MeshSupport "Build with Mesh support (requires fim library with trimesh2)" OFF)

## Weight Sampling term in optimization
## PRATEEP : Turned off (11/30)
#option(USE_WeighSampling "Weigh Sampling term in SW" OFF)

# Optionally build programs with GUIs.
option(BUILD_ShapeWorksView "Build ShapeWorksView (requires FLTK & VTK)" OFF)
option(BUILD_ShapeWorksView2 "Build ShapeWorksView2 (requires QT & VTK)" OFF)
#option(BUILD_ShapeWorksView "Build ShapeWorksView (requires FLTK & VTK)" ON)
#option(BUILD_ShapeWorksView2 "Build ShapeWorksView2 (requires QT & VTK)" ON)
option(BUILD_ShapeWorksShop "Build ShapeWorksShop (requires FLTK & VTK)" OFF)
option(BUILD_Unsupported "Build unsupported tools (NOT recommended, requires FLTK & VTK)" OFF)
option(CMAKE_BUILD_TYPE "Build Type" DEBUG)
option(BUILD_ICPRigid3DRegistration "Build ICPRigid3DRegistration (requires VTK)" ON)

# Optionally use OpenMP
option(USE_OPENMP "Build parallel optimization using OpenMP" ON)
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

# Configure Mesh support
if(BUILD_MeshSupport)
  SUBDIRS(fim)
  INCLUDE_DIRECTORIES(fim/trimesh2/include)
  add_definitions(-DSW_USE_MESH)
endif(BUILD_MeshSupport)

## Use weigh sampling term
## PRATEEP : Turned off (11/30)
#if(USE_WeighSampling)
#  add_definitions(-DSW_WEIGH_SAMPLING_TERM)
#endif(USE_WeighSampling)

## Use correspondence term
## PRATEEP : Turned off (11/30)
#option(USE_CORR "Use correspondence" ON)
#if(USE_CORR)
#  add_definitions(-DUSE_CORR)
#endif(USE_CORR)

# Set up the include directories
include_directories ( 
${SHAPEWORKS_SOURCE_DIR}/Utilities
${SHAPEWORKS_SOURCE_DIR}/ITKParticleSystem 
${SHAPEWORKS_SOURCE_DIR}/tinyxml
)

# Command-line tools are built by default.
subdirs(tinyxml ITKParticleSystem Utilities ShapeWorksRun ShapeWorksGroom)

if (BUILD_ShapeWorksView2)
 set(USE_QT 1)
 set(USE_VTK 1)
endif(BUILD_ShapeWorksView2)

if (BUILD_ShapeWorksShop OR BUILD_ShapeWorksView OR BUILD_Unsupported)
 set(USE_VTK 1)
 set(USE_FLTK 1)
endif(BUILD_ShapeWorksShop OR BUILD_ShapeWorksView OR BUILD_Unsupported)

if (BUILD_ICPRigid3DRegistration) 
 set(USE_VTK 1)
endif(BUILD_ICPRigid3DRegistration)

# Optionally configure VTK
# OPTION(USE_VTK "Use VTK (The Visualization Toolkit). VTK is required to build ShapeWorksView and ShapeWorksShop." OFF)
IF(USE_VTK)
  FIND_PACKAGE(VTK REQUIRED)
  IF (VTK_FOUND)
    INCLUDE (${VTK_USE_FILE})
  ELSE(VTK_FOUND)
    MESSAGE(FATAL_ERROR "VTK (The Visualization Toolkit) is required for ShapeWorksView and ShapeWorksShop, but could not be found.")
  ENDIF (VTK_FOUND)
ENDIF(USE_VTK)

IF(USE_QT)
  #--------------------------------------------------------------------------------
  # This sets the windows build that will need the special winmain@16 call. Qt provides
  #  this for us in the qtmain.lib file. Using this cmake code will ensure we have it
  #  linked into our build. Not needed on Unix/OS X/Linux which is why we have the 
  #  IF(WIN32) conditional.
  IF (WIN32)
    SET (QT_USE_QTMAIN TRUE)
  ENDIF (WIN32)
  
  # this command finds Qt4 libraries and sets all required variables
  # note that it's Qt4, not QT4 or qt4
  SET(QT_MIN_VERSION "4.7.4")
  FIND_PACKAGE( Qt4 REQUIRED )
  
  # add some useful macros and variables
  # (QT_USE_FILE is a variable defined by FIND_PACKAGE( Qt4 ) that contains a path to CMake script)
  INCLUDE( ${QT_USE_FILE} )
  
ENDIF(USE_QT)

# Optionally configure FLTK
IF(USE_FLTK)
  SET(FLTK_SKIP_FORMS 1)
  SET(FLTK_SKIP_IMAGES 1)
  FIND_PACKAGE(FLTK)

#  MESSAGE (${FLTK_USE_FILE})


  IF(FLTK_FOUND)
    INCLUDE_DIRECTORIES(${FLTK_INCLUDE_DIR})
  ELSE(FLTK_FOUND)
    MESSAGE(FATAL_ERROR "FLTK (The Fast Light Toolkit) is required, but could not be found.")
  ENDIF(FLTK_FOUND)
ENDIF(USE_FLTK)

# optionally build with powercrust reconstruction
OPTION(BUILD_PowerCrust "Build with powercrust reconstruction (requires powercrust library)" OFF)
IF(BUILD_PowerCrust)
  ADD_DEFINITIONS(-DSW_USE_POWERCRUST)
ENDIF(BUILD_PowerCrust)

IF (VTK_FOUND)
  IF(BUILD_ShapeWorksView2)
    SUBDIRS(ShapeWorksView2)
  ENDIF(BUILD_ShapeWorksView2)
 IF (FLTK_FOUND)
  INCLUDE_DIRECTORIES(${SHAPEWORKS_SOURCE_DIR}/VtkFltk)
  SUBDIRS(VtkFltk)
  IF (BUILD_ShapeWorksShop)
   SUBDIRS(ShapeWorksShop)
  ENDIF(BUILD_ShapeWorksShop)
  IF(BUILD_ShapeWorksView)
    SUBDIRS(ShapeWorksView)
    IF(BUILD_PowerCrust)
      INCLUDE(FindPowerCrust.cmake)
      IF(POWERCRUST_FOUND)
        INCLUDE_DIRECTORIES(${POWERCRUST_INCLUDE})
      ELSE(POWERCRUST_FOUND)
        MESSAGE(FATAL_ERROR "POWERCRUST library is required, but could not be found.")   
      ENDIF(POWERCRUST_FOUND)
    ENDIF(BUILD_PowerCrust)  
  ENDIF(BUILD_ShapeWorksView)
  IF(BUILD_Unsupported)
   SUBDIRS(Unsupported)
  ENDIF(BUILD_Unsupported)
 ENDIF(FLTK_FOUND)
ENDIF(VTK_FOUND)

IF (VTK_FOUND)
 IF (BUILD_ICPRigid3DRegistration)
	SUBDIRS(ICPRigid3DRegistration)
 ENDIF(BUILD_ICPRigid3DRegistration)
ENDIF(VTK_FOUND)

IF (WIN32 AND MSVC)
  ADD_DEFINITIONS(-D_BIND_TO_CURRENT_VCLIBS_VERSION=1)
ENDIF (WIN32 AND MSVC)

