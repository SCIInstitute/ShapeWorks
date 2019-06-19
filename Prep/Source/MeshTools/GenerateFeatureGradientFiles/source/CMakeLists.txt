project(GenerateFeatureGradientFiles)
cmake_minimum_required(VERSION 2.8)
add_executable(GenerateFeatureGradientFiles GenerateFeatureGradientFiles.cxx)
TARGET_LINK_LIBRARIES(GenerateFeatureGradientFiles ${ITK_LIBRARIES} fim_v4 tinyxml vgl vgl_algo)
INSTALL(TARGETS GenerateFeatureGradientFiles RUNTIME DESTINATION .)

add_executable(WriteFeatureGradientAsText WriteFeatureGradientAsText.cxx)
INSTALL(TARGETS WriteFeatureGradientAsText RUNTIME DESTINATION .)
