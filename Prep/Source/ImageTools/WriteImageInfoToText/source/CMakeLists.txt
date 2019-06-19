cmake_minimum_required(VERSION 2.8)

project(WriteImageInfoToText)
add_executable(WriteImageInfoToText WriteImageInfoToText.cxx ${source_files})
target_link_libraries(WriteImageInfoToText ${ITK_LIBRARIES})
INSTALL(TARGETS WriteImageInfoToText  RUNTIME DESTINATION .)
