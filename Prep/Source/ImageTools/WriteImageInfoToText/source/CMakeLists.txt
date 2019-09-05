cmake_minimum_required(VERSION 2.8)

project(WriteImageInfoToText)
add_executable(WriteImageInfoToText WriteImageInfoToText.cxx )
target_link_libraries(WriteImageInfoToText ${ITK_LIBRARIES} OptionParser)
INSTALL(TARGETS WriteImageInfoToText RUNTIME DESTINATION bin)
