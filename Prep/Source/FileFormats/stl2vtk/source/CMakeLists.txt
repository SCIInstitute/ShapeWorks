 
PROJECT(stl2vtk)
cmake_minimum_required(VERSION 2.6)
ADD_EXECUTABLE(stl2vtk stl2vtk.cxx ${source_files})
TARGET_LINK_LIBRARIES(stl2vtk ${VTK_LIBRARIES} )
INSTALL(TARGETS stl2vtk  RUNTIME DESTINATION .)
