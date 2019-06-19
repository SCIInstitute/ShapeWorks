 
project(GenerateBinaryAndDTImagesFromMeshes)
cmake_minimum_required(VERSION 2.8)
ADD_EXECUTABLE(GenerateBinaryAndDTImagesFromMeshes GenerateBinaryAndDTImagesFromMeshes.cxx ${source_files})
TARGET_LINK_LIBRARIES(GenerateBinaryAndDTImagesFromMeshes ${ITK_LIBRARIES} ${VTK_LIBRARIES} fim_v4 tinyxml vgl vgl_algo)
INSTALL(TARGETS GenerateBinaryAndDTImagesFromMeshes RUNTIME DESTINATION .)


