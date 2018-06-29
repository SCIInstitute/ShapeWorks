 
project(ComputeCurvatureAndCoordFiles)
cmake_minimum_required(VERSION 2.8)
ADD_EXECUTABLE(ComputeCurvatureAndCoordFiles ComputeCurvatureAndCoordFiles.cxx ${source_files})
TARGET_LINK_LIBRARIES(ComputeCurvatureAndCoordFiles ${ITK_LIBRARIES} ${VTK_LIBRARIES} vgl_algo vgl  fim_v4 tinyxml)
INSTALL(TARGETS ComputeCurvatureAndCoordFiles RUNTIME DESTINATION .)


