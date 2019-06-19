 
PROJECT(ProbeFeatureVolumesAtMeshVertices)
cmake_minimum_required(VERSION 2.8)
ADD_EXECUTABLE(ProbeFeatureVolumesAtMeshVertices ProbeFeatureVolumesAtMeshVertices.cxx)
TARGET_LINK_LIBRARIES(ProbeFeatureVolumesAtMeshVertices ${VTK_LIBRARIES}  ${ITK_LIBRARIES} tinyxml)
INSTALL(TARGETS ProbeFeatureVolumesAtMeshVertices  RUNTIME DESTINATION .)
