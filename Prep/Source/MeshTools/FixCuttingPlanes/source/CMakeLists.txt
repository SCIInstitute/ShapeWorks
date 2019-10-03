 
project(FixCuttingPlanes)
cmake_minimum_required(VERSION 2.8)
ADD_EXECUTABLE(FixCuttingPlanes FixCuttingPlanes.cxx )
TARGET_LINK_LIBRARIES(FixCuttingPlanes ${ITK_LIBRARIES} tinyxml)
INSTALL(TARGETS FixCuttingPlanes RUNTIME DESTINATION bin)


