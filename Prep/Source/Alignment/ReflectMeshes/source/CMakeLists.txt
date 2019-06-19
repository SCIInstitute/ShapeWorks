 
PROJECT(ReflectMeshes)
cmake_minimum_required(VERSION 2.8)
ADD_EXECUTABLE(ReflectMeshes ReflectMeshes.cxx ${source_files})
TARGET_LINK_LIBRARIES(ReflectMeshes ${VTK_LIBRARIES} tinyxml)
INSTALL(TARGETS ReflectMeshes  RUNTIME DESTINATION .)
