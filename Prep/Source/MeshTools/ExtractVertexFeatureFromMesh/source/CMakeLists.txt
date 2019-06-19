 
PROJECT(ExtractVertexFeatureFromMesh)
cmake_minimum_required(VERSION 2.6)
ADD_EXECUTABLE(ExtractVertexFeatureFromMesh ExtractVertexFeatureFromMesh.cxx ${source_files})
TARGET_LINK_LIBRARIES(ExtractVertexFeatureFromMesh ${VTK_LIBRARIES}  tinyxml)
INSTALL(TARGETS ExtractVertexFeatureFromMesh  RUNTIME DESTINATION .)
