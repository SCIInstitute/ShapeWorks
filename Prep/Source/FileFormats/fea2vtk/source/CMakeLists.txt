 
project(fea2vtk)
cmake_minimum_required(VERSION 2.8)
ADD_EXECUTABLE(fea2vtk fea2vtk.cxx)
TARGET_LINK_LIBRARIES(fea2vtk ${ITK_LIBRARIES} ${VTK_LIBRARIES} fim_v4 tinyxml vgl vgl_algo)
INSTALL(TARGETS fea2vtk RUNTIME DESTINATION .)


