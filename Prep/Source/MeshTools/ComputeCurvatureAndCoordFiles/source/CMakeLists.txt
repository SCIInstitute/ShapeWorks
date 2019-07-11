 
project(ComputeCurvatureAndCoordFiles)
cmake_minimum_required(VERSION 2.8)
ADD_EXECUTABLE(ComputeCurvatureAndCoordFiles ComputeCurvatureAndCoordFiles.cxx ${source_files})
TARGET_LINK_LIBRARIES(ComputeCurvatureAndCoordFiles ${ITK_LIBRARIES} ${VTK_LIBRARIES} fim_v4 vgl_algo vgl tinyxml)
INSTALL(TARGETS ComputeCurvatureAndCoordFiles RUNTIME DESTINATION .)


