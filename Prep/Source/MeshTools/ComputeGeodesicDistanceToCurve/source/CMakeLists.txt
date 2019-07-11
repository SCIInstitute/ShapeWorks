 
project(ComputeGeodesicDistanceToCurve)
cmake_minimum_required(VERSION 2.8)
ADD_EXECUTABLE(ComputeGeodesicDistanceToCurve ComputeGeodesicDistanceToCurve.cxx ${source_files})
TARGET_LINK_LIBRARIES(ComputeGeodesicDistanceToCurve ${ITK_LIBRARIES} ${VTK_LIBRARIES} fim_v4 vgl_algo vgl tinyxml)
INSTALL(TARGETS ComputeGeodesicDistanceToCurve RUNTIME DESTINATION .)


