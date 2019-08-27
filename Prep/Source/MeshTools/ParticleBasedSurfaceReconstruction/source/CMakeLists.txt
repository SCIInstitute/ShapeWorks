
PROJECT(ParticleBasedSurfaceReconstruction)
cmake_minimum_required(VERSION 2.6)

ADD_EXECUTABLE(ReconstructMeanSurface ReconstructMeanSurface.cxx)
TARGET_LINK_LIBRARIES(ReconstructMeanSurface ${VTK_LIBRARIES}  ${ITK_LIBRARIES} Transforms SurfaceReconstruction tinyxml Utils)
INSTALL(TARGETS ReconstructMeanSurface  RUNTIME DESTINATION .)

#PROJECT(ParticleBasedSurfaceReconstruction)
#cmake_minimum_required(VERSION 2.6)

##ctc: don't try to add include directories like this, but instead use higher level definitions and implicit inclusion from target_link_libraries
#SET(COMMON_LIBS_DIR "../../../CommonLibraries")

#option(USE_TPS "Warp using Thin Plate Splines" OFF)
#if(USE_TPS)
#  add_definitions(-DUSE_TPS)
#ENDIF(USE_TPS)

#option(VIS_STEPS "Visualize intermediate steps (for debugging)" OFF)
#if(VIS_STEPS)
#  add_definitions(-DVIS_DEBUG)
#ENDIF(VIS_STEPS)

#IF (WIN32 AND MSVC)
#  ADD_DEFINITIONS(-D_SCL_SECURE_NO_WARNINGS)
#  ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
#ELSE()
#  set(CMAKE_CXX_FLAGS "-lboost_system -lboost_thread -fopenmp")
#  set(CMAKE_C_FLAGS "-lboost_system -lboost_thread -fopenmp")
#ENDIF (WIN32 AND MSVC)

#cmake_policy(SET CMP0020 NEW)

##--------------------------------------------------------------------------------
## ITK, VTK
##--------------------------------------------------------------------------------
#FIND_PACKAGE(VTK COMPONENTS
#  vtkCommonCore
#  vtkInfovisCore
#  vtkInteractionStyle
#  vtkViewsQt
#  vtkFiltersCore
#  vtkGUISupportQt
#  vtkRenderingQt
#  vtkIOLegacy
#  vtkIOPLY
#  )
#INCLUDE( ${VTK_USE_FILE} )
#MESSAGE(STATUS "** VTK_DIR: ${VTK_DIR}")

#FIND_PACKAGE(ITK)
#IF (ITK_FOUND)
#  INCLUDE (${ITK_USE_FILE})
#ENDIF(ITK_FOUND)
#MESSAGE(STATUS "** ITK_DIR: ${ITK_DIR}")

#if (NOT WIN32)
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC")
#endif()

#INCLUDE_DIRECTORIES(
#  ${COMMON_LIBS_DIR}/Eigen
#  ${surfworks_SOURCE_DIR}/Params
#  ${surfworks_SOURCE_DIR}/Transforms
#  ${surfworks_SOURCE_DIR}/Utils
#  )


## Make the h/hpp files appear in a QtCreator project
#file(
#  GLOB
#  source_files
#  Params/*.h
#  Params/*.hxx
#  Transforms/*.h
#  Transforms/*.hxx
#  Utils/*.h
#  Utils/*.cpp
#  )

#SET( BIN_LIST
#  WarpToMeanSpace
#  #WarpToMeanSpaceWithPreviewMeshQC
#  #WarpToSubjectSpace
#  #MoveAlongPCAModes
#  )

#FOREACH( var ${BIN_LIST} )

#  if(USE_TPS)
#    if(VIS_STEPS)
#      ADD_EXECUTABLE( ${var}TPSDebug ${var}.cxx ${source_files})
#      TARGET_LINK_LIBRARIES(${var}TPSDebug ${VTK_LIBRARIES} ${ITK_LIBRARIES} tinyxml PreViewMeshQC Procrustes)
#      INSTALL(TARGETS ${var}TPSDebug  RUNTIME DESTINATION .)
#    else(VIS_STEPS)
#      ADD_EXECUTABLE( ${var}TPS ${var}.cxx ${source_files})
#      TARGET_LINK_LIBRARIES(${var}TPS ${VTK_LIBRARIES} ${ITK_LIBRARIES} tinyxml PreViewMeshQC Procrustes)
#      INSTALL(TARGETS ${var}TPS  RUNTIME DESTINATION .)
#    endif(VIS_STEPS)
#  else(USE_TPS)
#    if(VIS_STEPS)
#      ADD_EXECUTABLE( ${var}RBFDebug ${var}.cxx ${source_files})
#      TARGET_LINK_LIBRARIES(${var}RBFDebug ${VTK_LIBRARIES} ${ITK_LIBRARIES} tinyxml PreViewMeshQC Procrustes)
#      INSTALL(TARGETS ${var}RBFDebug  RUNTIME DESTINATION .)
#    else(VIS_STEPS)
#      ADD_EXECUTABLE( ${var}RBF ${var}.cxx ${source_files})
#      TARGET_LINK_LIBRARIES(${var}RBF ${VTK_LIBRARIES} ${ITK_LIBRARIES} tinyxml PreViewMeshQC Procrustes)
#      INSTALL(TARGETS ${var}RBF  RUNTIME DESTINATION .)
#    endif(VIS_STEPS)

#  endif(USE_TPS)

#ENDFOREACH( var )


