PROJECT(SHAPEWORKS)

cmake_minimum_required(VERSION 2.6)



#Match everything when analyzing dependencies

INCLUDE_REGULAR_EXPRESSION("^.*$")



# Configure ITK -- REQUIRED
#INCLUDE (${CMAKE_ROOT}/Modules/FindITK.cmake)
#SET(USE_ITK_FILE 1)

#IF (USE_ITK_FILE)
 # INCLUDE(${USE_ITK_FILE})
#ELSE(USE_ITK_FILE)
 # MESSAGE(SEND_ERROR "ITK (The Insight Toolkit) is required, but could not be found.")
#ENDIF(USE_ITK_FILE)

FIND_PACKAGE(ITK REQUIRED)
IF(ITK_FOUND)
  INCLUDE(${USE_ITK_FILE})
ELSE(ITK_FOUND)
  MESSAGE(SEND_ERROR "ITK (The Insight Toolkit) is required, but could not be found.")

ENDIF(ITK_FOUND)



INCLUDE_DIRECTORIES(./trimesh2/include ./trimesh2/include/GL ./trimesh2/gluit ./trimesh2/utilsrc ./fim)



FILE(GLOB gluit_srcs ./trimesh2/gluit/freeglut_*.c* ./trimesh2/gluit/glui*.cc)

ADD_LIBRARY (freeglut_static STATIC ${gluit_srcs})



FILE(GLOB trimesh_srcs ./trimesh2/libsrc/*.cc)

ADD_LIBRARY (trimesh STATIC ${trimesh_srcs})



# add fim solver library

FILE(GLOB eikonal_srcs ./fim/*.c*)

FILE(GLOB eikonal_hdrs ./fim/*.h)

ADD_LIBRARY(fim_lib STATIC ${eikonal_hdrs} ${eikonal_srcs})



# Choose what to build.  Command-line tools are build by default.

SUBDIRS(params ITKParticleSystem Utilities ShapeWorksRun ShapeWorksGroom)



# Optionally build programs with GUIs.

OPTION(BUILD_ShapeWorksView "Build ShapeWorksView (requires FLTK & VTK)" OFF)

OPTION(BUILD_ShapeWorksShop "Build ShapeWorksShop (requires FLTK & VTK)" OFF)

OPTION(BUILD_Unsupported "Build unsupported tools (NOT recommended, requires FLTK & VTK)" OFF)
OPTION(BUILD_ICPRigid3DRegistration "Build ICPRigid3DRegistration (requires VTK)" OFF)

IF (BUILD_ShapeWorksShop OR BUILD_ShapeWorksView OR BUILD_Unsupported) 

 SET(USE_VTK 1)

 SET(USE_FLTK 1)

ENDIF(BUILD_ShapeWorksShop OR BUILD_ShapeWorksView OR BUILD_Unsupported)

IF (BUILD_ICPRigid3DRegistration) 
 SET(USE_VTK 1)
ENDIF(BUILD_ICPRigid3DRegistration)

# Optionally configure VTK

# OPTION(USE_VTK "Use VTK (The Visualization Toolkit). VTK is required to build ShapeWorksView and ShapeWorksShop." OFF)

IF(USE_VTK)
  FIND_PACKAGE(VTK REQUIRED)
  IF (VTK_FOUND)

    INCLUDE (${VTK_USE_FILE})

  ELSE(VTK_FOUND)

    MESSAGE(SEND_ERROR "VTK (The Visualization Toolkit) is required, but could not be found.")

  ENDIF (VTK_FOUND)

ENDIF(USE_VTK)



# Optionally configure FLTK

IF(USE_FLTK)

  SET(FLTK_SKIP_FORMS 1)

  SET(FLTK_SKIP_IMAGES 1)

  FIND_PACKAGE(FLTK)

  IF(FLTK_FOUND)

    INCLUDE_DIRECTORIES(${FLTK_INCLUDE_DIR})

  ELSE(FLTK_FOUND)

    MESSAGE(SEND_ERROR "FLTK (The Fast Light Toolkit) is required, but could not be found.")

  ENDIF(FLTK_FOUND)

ENDIF(USE_FLTK)



INCLUDE_DIRECTORIES ( 

${SHAPEWORKS_SOURCE_DIR}/Utilities

${SHAPEWORKS_SOURCE_DIR}/ITKParticleSystem 

${SHAPEWORKS_SOURCE_DIR}/params

)



IF (VTK_FOUND)

 IF (FLTK_FOUND)

  INCLUDE_DIRECTORIES(${SHAPEWORKS_SOURCE_DIR}/VtkFltk)

  SUBDIRS(VtkFltk)

  IF (BUILD_ShapeWorksShop)

   SUBDIRS(ShapeWorksShop)

  ENDIF(BUILD_ShapeWorksShop)

  IF(BUILD_ShapeWorksView)

   SUBDIRS(ShapeWorksView)

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
