# still can't find gtest/gtest.h, not sure why
# needed to test private functions
#find_package(GTest REQUIRED)

# Mesh library

set(Mesh_sources
 Mesh.cpp
 meshFIM.cpp
 MeshUtils.cpp
 MeshWarper.cpp
 )

set(Mesh_headers
 Mesh.h
 meshFIM.h
 MeshUtils.h
 MeshWarper.h
 )

FILE(GLOB PreviewMeshQC_sources ./PreviewMeshQC/*.cpp)
FILE(GLOB PreviewMeshQC_headers ./PreviewMeshQC/*.h)

add_library(Mesh STATIC
 ${Mesh_sources}
 ${Mesh_headers}
 ${PreviewMeshQC_sources}
 ${PreviewMeshQC_headers}
 )

target_include_directories(Mesh PUBLIC
 $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
 $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PreviewMeshQC>
 $<INSTALL_INTERFACE:include>
 )

target_link_libraries(
  Mesh 
  Image
  Common
  Utils
  trimesh2 
  vgl 
  vgl_algo 
  ${VTK_LIBRARIES}
  Eigen3::Eigen
  igl::core
  Particles
  TBB::tbb
  geometry-central
  Boost::filesystem
  )

# Install
set_target_properties(Mesh PROPERTIES PUBLIC_HEADER
 "${Mesh_headers};${PreviewMeshQC_headers}"
 )

install(TARGETS Mesh
 ARCHIVE DESTINATION lib
 RUNTIME DESTINATION lib
 LIBRARY DESTINATION lib
 PUBLIC_HEADER DESTINATION include
 )

#ADD_EXECUTABLE(ComputeGeodesicDistanceToCurve ComputeGeodesicDistanceToCurve.cxx)
#TARGET_LINK_LIBRARIES(ComputeGeodesicDistanceToCurve ${ITK_LIBRARIES} Mesh tinyxml)
#INSTALL(TARGETS ComputeGeodesicDistanceToCurve RUNTIME DESTINATION bin)

# Added to backlog
#ADD_EXECUTABLE(GenerateFidsFiles GenerateFidsFiles.cxx)
#TARGET_LINK_LIBRARIES(GenerateFidsFiles ${ITK_LIBRARIES} Mesh tinyxml)
#INSTALL(TARGETS GenerateFidsFiles RUNTIME DESTINATION bin)

#ADD_EXECUTABLE(ComputeCurvatureAndCoordFiles ComputeCurvatureAndCoordFiles.cxx)
#TARGET_LINK_LIBRARIES(ComputeCurvatureAndCoordFiles ${ITK_LIBRARIES} Mesh tinyxml)
#INSTALL(TARGETS ComputeCurvatureAndCoordFiles RUNTIME DESTINATION bin)

#ADD_EXECUTABLE(ComputeGeodesicDistanceToLandmark ComputeGeodesicDistanceToLandmark.cxx)
#TARGET_LINK_LIBRARIES(ComputeGeodesicDistanceToLandmark ${ITK_LIBRARIES} ${VXL_LIBRARIES} Mesh tinyxml)
#INSTALL(TARGETS ComputeGeodesicDistanceToLandmark RUNTIME DESTINATION bin)

ADD_EXECUTABLE(ReconstructMeanSurface ReconstructMeanSurface.cxx)
TARGET_LINK_LIBRARIES(ReconstructMeanSurface ${ITK_LIBRARIES} tinyxml Utils Analyze)
INSTALL(TARGETS ReconstructMeanSurface RUNTIME DESTINATION bin)

ADD_EXECUTABLE(ReconstructSurface ReconstructSurface.cxx)
TARGET_LINK_LIBRARIES(ReconstructSurface ${ITK_LIBRARIES} tinyxml Utils Analyze)
INSTALL(TARGETS ReconstructSurface RUNTIME DESTINATION bin)

ADD_EXECUTABLE(ReconstructSamplesAlongPCAModes ReconstructSamplesAlongPCAModes.cxx)
TARGET_LINK_LIBRARIES(ReconstructSamplesAlongPCAModes ${ITK_LIBRARIES} tinyxml Utils Analyze Optimize Particles)
INSTALL(TARGETS ReconstructSamplesAlongPCAModes RUNTIME DESTINATION bin)

# Added to backlog
#ADD_EXECUTABLE(GenerateFidsFilesFromMeshes GenerateFidsFilesFromMeshes.cxx)
#TARGET_LINK_LIBRARIES(GenerateFidsFilesFromMeshes ${ITK_LIBRARIES} Mesh tinyxml)
#INSTALL(TARGETS GenerateFidsFilesFromMeshes RUNTIME DESTINATION bin)

# Added to backlog
ADD_EXECUTABLE(RemoveFidsDTLeakage RemoveFidsDTLeakage.cxx)
TARGET_LINK_LIBRARIES(RemoveFidsDTLeakage ${ITK_LIBRARIES} Mesh tinyxml)
INSTALL(TARGETS RemoveFidsDTLeakage RUNTIME DESTINATION bin)
