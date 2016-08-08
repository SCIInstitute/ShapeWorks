PROJECT( ShapeWorksStudio )

cmake_minimum_required(VERSION 2.8)  

set(CMAKE_BUILD_TYPE Debug)
IF (WIN32 AND MSVC)
  ADD_DEFINITIONS(-D_SCL_SECURE_NO_WARNINGS)
  ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
ENDIF (WIN32 AND MSVC)

#--------------------------------------------------------------------------------
# ITK, VTK
#--------------------------------------------------------------------------------
FIND_PACKAGE(VTK COMPONENTS
  vtkCommonCore
  vtkInfovisCore
  vtkInteractionStyle
  vtkViewsQt
  vtkFiltersCore
  vtkGUISupportQt
  vtkRenderingQt
  vtkIOLegacy
)
INCLUDE( ${VTK_USE_FILE} )
MESSAGE(STATUS "** VTK_DIR: ${VTK_DIR}")

FIND_PACKAGE(ITK REQUIRED)
IF (ITK_FOUND)
  INCLUDE (${ITK_USE_FILE})
ENDIF(ITK_FOUND)
MESSAGE(STATUS "** ITK_DIR: ${ITK_DIR}")

if (NOT WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC")
endif()

#--------------------------------------------------------------------------------
### Projects
#--------------------------------------------------------------------------------

add_subdirectory(ParticleShapeworks)
add_subdirectory(Groom)
add_subdirectory(Optimize)
add_subdirectory(Surfworks)
add_subdirectory(TinyXML)
add_subdirectory(Application)
add_subdirectory(CLT)
