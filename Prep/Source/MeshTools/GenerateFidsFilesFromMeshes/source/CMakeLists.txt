 
project(GenerateFidsFilesFromMeshes)
cmake_minimum_required(VERSION 2.8)
ADD_EXECUTABLE(GenerateFidsFilesFromMeshes GenerateFidsFilesFromMeshes.cxx )
TARGET_LINK_LIBRARIES(GenerateFidsFilesFromMeshes ${ITK_LIBRARIES} ${VTK_LIBRARIES} fim_v4 tinyxml vgl vgl_algo)
INSTALL(TARGETS GenerateFidsFilesFromMeshes RUNTIME DESTINATION bin)


