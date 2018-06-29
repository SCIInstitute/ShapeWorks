 
project(ComputeGeodesicDistanceToCurve)
cmake_minimum_required(VERSION 2.8)
ADD_EXECUTABLE(ComputeGeodesicDistanceToCurve ComputeGeodesicDistanceToCurve.cxx ${source_files})
TARGET_LINK_LIBRARIES(ComputeGeodesicDistanceToCurve ${ITK_LIBRARIES} ${VTK_LIBRARIES} vgl_algo vgl  fim_v4 tinyxml)
INSTALL(TARGETS ComputeGeodesicDistanceToCurve RUNTIME DESTINATION .)


