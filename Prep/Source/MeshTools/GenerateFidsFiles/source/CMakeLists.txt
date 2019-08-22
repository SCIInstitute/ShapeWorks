 
project(GenerateFidsFiles)
cmake_minimum_required(VERSION 2.8)
ADD_EXECUTABLE(GenerateFidsFiles GenerateFidsFiles.cxx )
TARGET_LINK_LIBRARIES(GenerateFidsFiles ${ITK_LIBRARIES} ${VTK_LIBRARIES} fim_v4 tinyxml vgl vgl_algo)
INSTALL(TARGETS GenerateFidsFiles RUNTIME DESTINATION .)


