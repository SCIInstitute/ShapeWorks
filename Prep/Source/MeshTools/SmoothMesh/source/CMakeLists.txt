 
PROJECT(SmoothMesh)
cmake_minimum_required(VERSION 2.8)
ADD_EXECUTABLE(SmoothMesh SmoothMesh.cxx ${source_files})
TARGET_LINK_LIBRARIES(SmoothMesh ${VTK_LIBRARIES}  tinyxml)
INSTALL(TARGETS SmoothMesh  RUNTIME DESTINATION .)
