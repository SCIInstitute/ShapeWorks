PROJECT(SHAPEWORKS)
cmake_minimum_required(VERSION 2.6)

#Match everything when analyzing dependencies
INCLUDE_REGULAR_EXPRESSION("^.*$")

# Configure ITK -- REQUIRED
find_package(ITK REQUIRED)
if(ITK_FOUND)
  include(${USE_ITK_FILE})
else(ITK_FOUND)
  message(SEND_ERROR "ITK (The Insight Toolkit) is required, but could not be found.")
endif(ITK_FOUND)

# Optionally build with mesh support (requires linking against trimesh library)
option(BUILD_MeshSupport "Build with Mesh support (requires fim library with trimesh2)" OFF)

# Optionally build programs with GUIs.
option(BUILD_ShapeWorksView "Build ShapeWorksView (requires FLTK & VTK)" OFF)
option(BUILD_ShapeWorksShop "Build ShapeWorksShop (requires FLTK & VTK)" OFF)
option(BUILD_Unsupported "Build unsupported tools (NOT recommended, requires FLTK & VTK)" OFF)
option(BUILD_ICPRigid3DRegistration "Build ICPRigid3DRegistration (requires VTK)" OFF)

# Configure Mesh support
if(BUILD_MeshSupport)
include(FindFIM.cmake)

if(FIM_FOUND)
  include_directories(${FIM_INCLUDE})
  add_definitions(-DSW_USE_MESH)
else(FIM_FOUND)
 message(FATAL_ERROR "FIM library is required for mesh support, but could not be found.")   
endif(FIM_FOUND)

endif(BUILD_MeshSupport)

# Set up the include directories
include_directories ( 
${SHAPEWORKS_SOURCE_DIR}/Utilities
${SHAPEWORKS_SOURCE_DIR}/ITKParticleSystem 
${SHAPEWORKS_SOURCE_DIR}/tinyxml
)


# Command-line tools are built by default.
subdirs(tinyxml ITKParticleSystem Utilities ShapeWorksRun ShapeWorksGroom)

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

# Optionally configure FLTK
IF(USE_FLTK)
  SET(FLTK_SKIP_FORMS 1)
  SET(FLTK_SKIP_IMAGES 1)
  FIND_PACKAGE(FLTK)

  IF(FLTK_FOUND)
    INCLUDE_DIRECTORIES(${FLTK_INCLUDE_DIR})
  ELSE(FLTK_FOUND)
    MESSAGE(FATAL_ERROR "FLTK (The Fast Light Toolkit) is required, but could not be found.")
  ENDIF(FLTK_FOUND)
ENDIF(USE_FLTK)

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



