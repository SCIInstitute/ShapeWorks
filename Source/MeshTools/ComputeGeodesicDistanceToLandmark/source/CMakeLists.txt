 
project(ComputeGeodesicDistanceToLandmark)
cmake_minimum_required(VERSION 2.8)
ADD_EXECUTABLE(ComputeGeodesicDistanceToLandmark ComputeGeodesicDistanceToLandmark.cxx ${source_files})
TARGET_LINK_LIBRARIES(ComputeGeodesicDistanceToLandmark ${ITK_LIBRARIES} ${VTK_LIBRARIES} ${VXL_LIBRARIES} vgl_algo vgl fim_v4 tinyxml)
INSTALL(TARGETS ComputeGeodesicDistanceToLandmark RUNTIME DESTINATION .)
