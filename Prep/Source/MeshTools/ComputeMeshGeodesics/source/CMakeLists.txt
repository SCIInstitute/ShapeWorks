 
project(ComputeMeshGeodesics)
cmake_minimum_required(VERSION 2.8)
ADD_EXECUTABLE(ComputeMeshGeodesics ComputeMeshGeodesics.cxx ${source_files})
TARGET_LINK_LIBRARIES(ComputeMeshGeodesics ${ITK_LIBRARIES} ${VTK_LIBRARIES} fim_v4 vgl_algo vgl tinyxml)
INSTALL(TARGETS ComputeMeshGeodesics RUNTIME DESTINATION .)

