 
PROJECT(ClipClosedSurface)
cmake_minimum_required(VERSION 2.6)
ADD_EXECUTABLE(ClipClosedSurface ClipClosedSurface.cxx)
TARGET_LINK_LIBRARIES(ClipClosedSurface ${VTK_LIBRARIES}  ${ITK_LIBRARIES} tinyxml)
INSTALL(TARGETS ClipClosedSurface  RUNTIME DESTINATION bin)
