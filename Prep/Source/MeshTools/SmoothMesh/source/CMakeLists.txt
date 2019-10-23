 
PROJECT(SmoothMesh)
cmake_minimum_required(VERSION 2.8)
ADD_EXECUTABLE(SmoothMesh SmoothMesh.cxx )
TARGET_LINK_LIBRARIES(SmoothMesh ${VTK_LIBRARIES}  tinyxml)
INSTALL(TARGETS SmoothMesh  RUNTIME DESTINATION bin)
