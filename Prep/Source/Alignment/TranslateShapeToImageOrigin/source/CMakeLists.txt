cmake_minimum_required(VERSION 2.8)

project(TranslateShapeToImageOrigin)
add_executable(TranslateShapeToImageOrigin TranslateShapeToImageOrigin.cxx ${source_files})
target_link_libraries(TranslateShapeToImageOrigin ${ITK_LIBRARIES})
INSTALL(TARGETS TranslateShapeToImageOrigin  RUNTIME DESTINATION .)
