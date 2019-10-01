cmake_minimum_required(VERSION 2.8)

project(CropImages)
add_executable(CropImages CropImages.cxx)
target_link_libraries(CropImages ${ITK_LIBRARIES} OptionParser)
INSTALL(TARGETS CropImages  RUNTIME DESTINATION bin)

add_executable(CropRawImages CropRawImages.cxx)
target_link_libraries(CropRawImages ${ITK_LIBRARIES} OptionParser)
INSTALL(TARGETS CropRawImages  RUNTIME DESTINATION bin)

