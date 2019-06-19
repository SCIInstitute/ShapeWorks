 
PROJECT(VTKUnstructuredGridToPolyData)
cmake_minimum_required(VERSION 2.8)
ADD_EXECUTABLE(VTKUnstructuredGridToPolyData VTKUnstructuredGridToPolyData.cxx ${source_files})
TARGET_LINK_LIBRARIES(VTKUnstructuredGridToPolyData ${VTK_LIBRARIES} tinyxml)
INSTALL(TARGETS VTKUnstructuredGridToPolyData  RUNTIME DESTINATION .)
