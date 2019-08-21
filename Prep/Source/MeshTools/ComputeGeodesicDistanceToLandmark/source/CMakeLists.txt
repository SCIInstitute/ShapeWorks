 
project(ComputeGeodesicDistanceToLandmark)
cmake_minimum_required(VERSION 2.8)
ADD_EXECUTABLE(ComputeGeodesicDistanceToLandmark ComputeGeodesicDistanceToLandmark.cxx )
TARGET_LINK_LIBRARIES(ComputeGeodesicDistanceToLandmark ${ITK_LIBRARIES} ${VTK_LIBRARIES} ${VXL_LIBRARIES} fim_v4 vgl_algo vgl tinyxml)
INSTALL(TARGETS ComputeGeodesicDistanceToLandmark RUNTIME DESTINATION .)
