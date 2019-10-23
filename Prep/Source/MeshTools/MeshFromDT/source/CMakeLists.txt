
PROJECT(MeshFromDT)
cmake_minimum_required(VERSION 2.6)

ADD_EXECUTABLE(MeshFromDT main.cpp)
TARGET_LINK_LIBRARIES(MeshFromDT ${VTK_LIBRARIES} ${ITK_LIBRARIES} SurfaceReconstruction tinyxml Utils)
INSTALL(TARGETS MeshFromDT RUNTIME DESTINATION bin)
