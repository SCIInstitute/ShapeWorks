PROJECT(SHAPETOOLKIT)
cmake_minimum_required(VERSION 2.6)
SET(CMAKE_BUILD_TYPE RELEASE)
#SET(ITK_DIR "Path-to-ITK-4.7.2-build/")
#SET(VTK_DIR "Path-to-VTK-5.10.1-build")

#SET(VXL_DIR "Path-to-vxl-build")

SET(COMMON_LIBS_DIR "CommonLibraries")

FIND_PACKAGE(ITK REQUIRED)
IF (ITK_USE_FILE)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_USE_FILE)
  MESSAGE(SEND_ERROR "ITK (The Insight Toolkit) is required, but could not be found.")
ENDIF(ITK_USE_FILE)

FIND_PACKAGE(VTK)
IF (VTK_FOUND)
  INCLUDE (${VTK_USE_FILE})
ELSE(VTK_FOUND)
  MESSAGE(SEND_ERROR "VTK (The Visualization Toolkit) is required, but could not be found.")
ENDIF (VTK_FOUND)

FIND_PACKAGE(VXL)
IF (VXL_FOUND)
  INCLUDE (${VXL_DIR}/UseVXL.cmake)
ELSE(VTK_FOUND)
  MESSAGE(SEND_ERROR "VXL is required, but could not be found.")
ENDIF (VXL_FOUND)

# OpenMP -- REQUIRED
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()


INCLUDE_DIRECTORIES(
    ${COMMON_LIBS_DIR}/OptionParser
    ${COMMON_LIBS_DIR}/PreViewMeshQC
    ${COMMON_LIBS_DIR}/tinyxml
    ${COMMON_LIBS_DIR}/fim_v4
    ${COMMON_LIBS_DIR}/fim_v4/trimesh2/include
    ${COMMON_LIBS_DIR}/ITKVTK
    ${COMMON_LIBS_DIR}/Eigen
    ${COMMON_LIBS_DIR}/Procrustes
  )


SUBDIRS(${COMMON_LIBS_DIR}/fim_v4)
#INCLUDE_DIRECTORIES(${COMMON_LIBS_DIR}/fim_v4)
#INCLUDE_DIRECTORIES(${COMMON_LIBS_DIR}/fim_v4/trimesh2/include)
FILE(GLOB tinyxml_srcs ${COMMON_LIBS_DIR}/tinyxml/*.cpp)
ADD_LIBRARY (tinyxml STATIC ${tinyxml_srcs})

option(BUILD_ImageTools ON)
option(BUILD_MeshTools ON)
option(BUILD_FileFormatTools ON)
option(BUILD_AlignmentTools ON)
option(BUILD_DataFormatTools ON)

file(
	GLOB
	source_files
	*.h
	*.cpp
	*.hxx
	*.cxx
	*.txx
    ${COMMON_LIBS_DIR}/OptionParser/*.h
    ${COMMON_LIBS_DIR}/OptionParser/*.cpp
    ${COMMON_LIBS_DIR}/PreViewMeshQC/*.h
    ${COMMON_LIBS_DIR}/PreViewMeshQC/*.cpp
    ${COMMON_LIBS_DIR}/ITKVTK/*.h
  	${COMMON_LIBS_DIR}/ITKVTK/*.hxx
  	${COMMON_LIBS_DIR}/Procrustes/*.h
    ${COMMON_LIBS_DIR}/Procrustes/*.hxx
)

if (BUILD_ImageTools)
  subdirs(ImageTools/AntiAliasing/source)
  subdirs(ImageTools/CloseHoles/source)
  subdirs(ImageTools/CropImages/source)
  subdirs(ImageTools/ExtractGivenLabelImage/source)
  subdirs(ImageTools/PadVolumeWithConstant/source)
  subdirs(ImageTools/FindLargestBoundingBox/source)
  subdirs(ImageTools/ThresholdImages/source)
  subdirs(ImageTools/ReflectVolumes/source)
  subdirs(ImageTools/ResampleVolumesToBeIsotropic/source)
  subdirs(ImageTools/TopologyPreservingSmoothing/source)
  subdirs(ImageTools/WriteImageInfoToText/source)
  subdirs(ImageTools/ClipVolume/source)
endif(BUILD_ImageTools)

if (BUILD_MeshTools)
  subdirs(MeshTools/ClipClosedSurface/source)
  subdirs(MeshTools/ComputeCurvatureAndCoordFiles/source)
  subdirs(MeshTools/ComputeGeodesicDistanceToCurve/source)
  subdirs(MeshTools/ComputeGeodesicDistanceToLandmark/source)
  subdirs(MeshTools/ComputeGeodesicFromVerticesToPoints/source)
  subdirs(MeshTools/ComputeMeshGeodesics/source)
  subdirs(MeshTools/ComputeRasterizationVolumeOriginAndSize/source)
  #subdirs(MeshTools/ExtractDeepNetFeaturesForMeshVertices/source)
  subdirs(MeshTools/ExtractVertexFeatureFromMesh/source)
  subdirs(MeshTools/FeaFromVTK/source/FeaFromVTK)
  subdirs(MeshTools/FillMeshHoles/source)
  subdirs(MeshTools/FixCuttingPlanes/source)
  subdirs(MeshTools/GenerateBinaryAndDTImagesFromMeshes/source)
  subdirs(MeshTools/GenerateFeatureGradientFiles/source)
  subdirs(MeshTools/GenerateFidsFiles/source)
  subdirs(MeshTools/GenerateFidsFilesFromMeshes/source)
  subdirs(MeshTools/GetFeatureVolume/source)
#  subdirs(MeshTools/ParticleBasedSurfaceReconstruction/source)
  subdirs(MeshTools/PreviewCmd/source)
  subdirs(MeshTools/ProbeFeatureVolumesAtMeshVertices/source)
  subdirs(MeshTools/ProbeNormals/source)
  subdirs(MeshTools/ProjectPointsOntoMesh/source)
  subdirs(MeshTools/ReflectMeshes/source)
  subdirs(MeshTools/RemoveFidsDTLeakage/source)
  subdirs(MeshTools/SmoothMesh/source)
endif(BUILD_MeshTools)

if (BUILD_AlignmentTools)
  subdirs(Alignment/ICPRigid3DImageRegistration/source)
  subdirs(Alignment/ICPRigid3DMeshRegistration/source)
  subdirs(Alignment/ReflectMeshes/source)
  subdirs(Alignment/Resize_origin_resampleShapes)
  subdirs(Alignment/TranslateShapeToImageOrigin/source)
endif(BUILD_AlignmentTools)

if (BUILD_DataFormatTools)
  subdirs(DataFormat/VTKUnstructuredGridToPolyData/source)
endif(BUILD_DataFormatTools)

if (BUILD_DebugTools)
  subdirs(DebugTools/GradientFromMeshVsVolume/source)
#  subdirs(DebugTools/MeshBasedGeoToLMinShapeWorks)
endif(BUILD_DebugTools)

if (BUILD_FileFormatTools)
  subdirs(FileFormats/GenericMeshConversion/source)
  subdirs(FileFormats/fea2vtk/source)
  subdirs(FileFormats/ply2vtk/source)
  subdirs(FileFormats/stl2ply/source)
  subdirs(FileFormats/stl2vtk/source)
  subdirs(FileFormats/vtk2ply/source)
  subdirs(FileFormats/MeshFromDistanceTransforms/source)
endif(BUILD_FileFormatTools)
