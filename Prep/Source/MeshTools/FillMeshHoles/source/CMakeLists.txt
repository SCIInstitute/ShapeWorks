 
PROJECT(FillMeshHoles)
cmake_minimum_required(VERSION 2.6)
ADD_EXECUTABLE(FillMeshHoles FillMeshHoles.cxx ${source_files})
TARGET_LINK_LIBRARIES(FillMeshHoles ${VTK_LIBRARIES}  tinyxml)
INSTALL(TARGETS FillMeshHoles  RUNTIME DESTINATION .)
