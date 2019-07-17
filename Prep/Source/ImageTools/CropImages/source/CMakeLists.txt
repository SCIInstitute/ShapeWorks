cmake_minimum_required(VERSION 2.8)

project(CropImages)
add_executable(CropImages CropImages.cxx ${source_files})
target_link_libraries(CropImages ${ITK_LIBRARIES})
INSTALL(TARGETS CropImages  RUNTIME DESTINATION .)
