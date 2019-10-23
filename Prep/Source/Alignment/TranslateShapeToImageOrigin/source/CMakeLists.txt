cmake_minimum_required(VERSION 2.8)

project(TranslateShapeToImageOrigin)
add_executable(TranslateShapeToImageOrigin TranslateShapeToImageOrigin.cxx)
target_link_libraries(TranslateShapeToImageOrigin ${ITK_LIBRARIES} OptionParser)
INSTALL(TARGETS TranslateShapeToImageOrigin  RUNTIME DESTINATION bin)


add_executable(TranslateImages TranslateImages.cxx)
target_link_libraries(TranslateImages ${ITK_LIBRARIES} OptionParser)
INSTALL(TARGETS TranslateImages  RUNTIME DESTINATION bin)
