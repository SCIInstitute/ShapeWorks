cmake_minimum_required(VERSION 2.8)

project(TranslateImages)
add_executable(TranslateImages TranslateImages.cxx)
target_link_libraries(TranslateImages ${ITK_LIBRARIES} OptionParser)
INSTALL(TARGETS TranslateImages  RUNTIME DESTINATION bin)
