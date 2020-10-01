# Mesh library

set(Mesh_sources
 Mesh.cpp
 meshFIM.cpp
 MeshUtils.cpp
 GroomFemur.cpp
 )

set(Mesh_headers
 Mesh.h
 meshFIM.h
 MeshUtils.h
        BenchMeshFIM.cpp)

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
 $<INSTALL_INTERFACE:include>)

target_link_libraries(Mesh trimesh2 vgl vgl_algo ${VTK_LIBRARIES})

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
 
ADD_EXECUTABLE(GroomFemur GroomFemur.cpp)
TARGET_LINK_LIBRARIES(GroomFemur ${ITK_LIBRARIES} Mesh tinyxml)
INSTALL(TARGETS GroomFemur RUNTIME DESTINATION bin)

ADD_EXECUTABLE(GenerateBinaryAndDTImagesFromMeshes GenerateBinaryAndDTImagesFromMeshes.cxx )
TARGET_LINK_LIBRARIES(GenerateBinaryAndDTImagesFromMeshes ${ITK_LIBRARIES} Mesh tinyxml)
INSTALL(TARGETS GenerateBinaryAndDTImagesFromMeshes RUNTIME DESTINATION bin)

ADD_EXECUTABLE(ComputeMeshGeodesics ComputeMeshGeodesics.cxx )
TARGET_LINK_LIBRARIES(ComputeMeshGeodesics ${ITK_LIBRARIES} Mesh tinyxml)
INSTALL(TARGETS ComputeMeshGeodesics RUNTIME DESTINATION bin)

ADD_EXECUTABLE(ExtractVertexFeatureFromMesh ExtractVertexFeatureFromMesh.cxx )
TARGET_LINK_LIBRARIES(ExtractVertexFeatureFromMesh Mesh tinyxml)
INSTALL(TARGETS ExtractVertexFeatureFromMesh RUNTIME DESTINATION bin)

# Uses custom TriMesh functions without making an instance of meshFIM
#ADD_EXECUTABLE(GetFeatureVolume GetFeatureVolume.cxx)
#TARGET_LINK_LIBRARIES(GetFeatureVolume ${ITK_LIBRARIES} ${VXL_LIBRARIES} Mesh tinyxml)
#INSTALL(TARGETS GetFeatureVolume RUNTIME DESTINATION bin)

#ADD_EXECUTABLE(ComputeGeodesicDistanceToCurve ComputeGeodesicDistanceToCurve.cxx)
#TARGET_LINK_LIBRARIES(ComputeGeodesicDistanceToCurve ${ITK_LIBRARIES} Mesh tinyxml)
#INSTALL(TARGETS ComputeGeodesicDistanceToCurve RUNTIME DESTINATION bin)

#ADD_EXECUTABLE(GenerateFidsFiles GenerateFidsFiles.cxx )
#TARGET_LINK_LIBRARIES(GenerateFidsFiles ${ITK_LIBRARIES} Mesh tinyxml)
#INSTALL(TARGETS GenerateFidsFiles RUNTIME DESTINATION bin)

ADD_EXECUTABLE(ClipClosedSurface ClipClosedSurface.cxx)
TARGET_LINK_LIBRARIES(ClipClosedSurface ${ITK_LIBRARIES} Mesh tinyxml)
INSTALL(TARGETS ClipClosedSurface RUNTIME DESTINATION bin)

#ADD_EXECUTABLE(ComputeCurvatureAndCoordFiles ComputeCurvatureAndCoordFiles.cxx)
#TARGET_LINK_LIBRARIES(ComputeCurvatureAndCoordFiles ${ITK_LIBRARIES} Mesh tinyxml)
#INSTALL(TARGETS ComputeCurvatureAndCoordFiles RUNTIME DESTINATION bin)

#ADD_EXECUTABLE(ProjectPointsOntoMesh ProjectPointsOntoMesh.cxx)
#TARGET_LINK_LIBRARIES(ProjectPointsOntoMesh ${ITK_LIBRARIES} Mesh tinyxml)
#INSTALL(TARGETS ProjectPointsOntoMesh RUNTIME DESTINATION bin)

#add_executable(GenerateFeatureGradientFiles GenerateFeatureGradientFiles.cxx)
#TARGET_LINK_LIBRARIES(GenerateFeatureGradientFiles ${ITK_LIBRARIES} Mesh tinyxml)
#INSTALL(TARGETS GenerateFeatureGradientFiles RUNTIME DESTINATION bin)

#add_executable(WriteFeatureGradientAsText WriteFeatureGradientAsText.cxx)
#INSTALL(TARGETS WriteFeatureGradientAsText RUNTIME DESTINATION bin)

add_executable(ProbeNormals ProbeNormals.cxx)
TARGET_LINK_LIBRARIES(ProbeNormals ${ITK_LIBRARIES} Utils)
INSTALL(TARGETS ProbeNormals RUNTIME DESTINATION bin)

add_executable(ComputeMeanNormals ComputeMeanNormals.cxx)
TARGET_LINK_LIBRARIES(ComputeMeanNormals ${ITK_LIBRARIES} tinyxml Utils)
INSTALL(TARGETS ComputeMeanNormals RUNTIME DESTINATION bin)

#add_executable(GenerateNormalFeaFiles GenerateNormalFeaFiles.cxx)
#TARGET_LINK_LIBRARIES(GenerateNormalFeaFiles ${ITK_LIBRARIES} tinyxml Mesh)
#INSTALL(TARGETS GenerateNormalFeaFiles RUNTIME DESTINATION bin)

#ADD_EXECUTABLE(ComputeGeodesicDistanceToLandmark ComputeGeodesicDistanceToLandmark.cxx )
#TARGET_LINK_LIBRARIES(ComputeGeodesicDistanceToLandmark ${ITK_LIBRARIES} ${VXL_LIBRARIES} Mesh tinyxml)
#INSTALL(TARGETS ComputeGeodesicDistanceToLandmark RUNTIME DESTINATION bin)

ADD_EXECUTABLE(PreviewCmd PreviewCmd.cxx )
TARGET_LINK_LIBRARIES(PreviewCmd Utils Mesh )
INSTALL(TARGETS PreviewCmd RUNTIME DESTINATION bin)

ADD_EXECUTABLE(ReconstructMeanSurface ReconstructMeanSurface.cxx)
TARGET_LINK_LIBRARIES(ReconstructMeanSurface ${ITK_LIBRARIES} tinyxml Utils Analyze)
INSTALL(TARGETS ReconstructMeanSurface RUNTIME DESTINATION bin)

ADD_EXECUTABLE(ReconstructSurface ReconstructSurface.cxx)
TARGET_LINK_LIBRARIES(ReconstructSurface ${ITK_LIBRARIES} tinyxml Utils Analyze)
INSTALL(TARGETS ReconstructSurface RUNTIME DESTINATION bin)

ADD_EXECUTABLE(ReconstructSamplesAlongPCAModes ReconstructSamplesAlongPCAModes.cxx)
TARGET_LINK_LIBRARIES(ReconstructSamplesAlongPCAModes ${ITK_LIBRARIES} tinyxml Utils Analyze Optimize Particles)
INSTALL(TARGETS ReconstructSamplesAlongPCAModes RUNTIME DESTINATION bin)

#ADD_EXECUTABLE(GenerateFidsFilesFromMeshes GenerateFidsFilesFromMeshes.cxx )
#TARGET_LINK_LIBRARIES(GenerateFidsFilesFromMeshes ${ITK_LIBRARIES} Mesh tinyxml)
#INSTALL(TARGETS GenerateFidsFilesFromMeshes RUNTIME DESTINATION bin)

ADD_EXECUTABLE(FixCuttingPlanes FixCuttingPlanes.cxx )
TARGET_LINK_LIBRARIES(FixCuttingPlanes ${ITK_LIBRARIES} Mesh tinyxml)
INSTALL(TARGETS FixCuttingPlanes RUNTIME DESTINATION bin)

#ADD_EXECUTABLE(ComputeGeodesicDistanceFromVerticesToPoints ComputeGeodesicDistanceFromVerticesToPoints.cxx )
#TARGET_LINK_LIBRARIES(ComputeGeodesicDistanceFromVerticesToPoints ${ITK_LIBRARIES} ${VXL_LIBRARIES} Mesh tinyxml)
#INSTALL(TARGETS ComputeGeodesicDistanceFromVerticesToPoints RUNTIME DESTINATION bin)

ADD_EXECUTABLE(FillMeshHoles FillMeshHoles.cxx )
TARGET_LINK_LIBRARIES(FillMeshHoles Mesh tinyxml)
INSTALL(TARGETS FillMeshHoles RUNTIME DESTINATION bin)

ADD_EXECUTABLE(RemoveFidsDTLeakage RemoveFidsDTLeakage.cxx )
TARGET_LINK_LIBRARIES(RemoveFidsDTLeakage ${ITK_LIBRARIES} Mesh tinyxml)
INSTALL(TARGETS RemoveFidsDTLeakage RUNTIME DESTINATION bin)

ADD_EXECUTABLE(ProbeFeatureVolumesAtMeshVertices ProbeFeatureVolumesAtMeshVertices.cxx)
TARGET_LINK_LIBRARIES(ProbeFeatureVolumesAtMeshVertices ${ITK_LIBRARIES} Mesh tinyxml)
INSTALL(TARGETS ProbeFeatureVolumesAtMeshVertices RUNTIME DESTINATION bin)

ADD_EXECUTABLE(SmoothMesh SmoothMesh.cxx )
TARGET_LINK_LIBRARIES(SmoothMesh Mesh tinyxml)
INSTALL(TARGETS SmoothMesh RUNTIME DESTINATION bin)

ADD_EXECUTABLE(ComputeRasterizationVolumeOriginAndSize ComputeRasterizationVolumeOriginAndSize.cxx )
TARGET_LINK_LIBRARIES(ComputeRasterizationVolumeOriginAndSize Mesh tinyxml)
INSTALL(TARGETS ComputeRasterizationVolumeOriginAndSize RUNTIME DESTINATION bin)

add_executable(ReflectMesh ReflectMesh.cxx )
target_link_libraries(ReflectMesh Mesh Utils)
INSTALL(TARGETS ReflectMesh RUNTIME DESTINATION bin)

#ADD_EXECUTABLE(fea2vtk fea2vtk.cxx)
#TARGET_LINK_LIBRARIES(fea2vtk ${ITK_LIBRARIES} Mesh tinyxml)
#INSTALL(TARGETS fea2vtk RUNTIME DESTINATION bin)

ADD_EXECUTABLE(BenchMeshFIM BenchMeshFIM.cpp)
TARGET_LINK_LIBRARIES(BenchMeshFIM Mesh tinyxml)
INSTALL(TARGETS BenchMeshFIM RUNTIME DESTINATION bin)
