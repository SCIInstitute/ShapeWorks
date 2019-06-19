cmake_minimum_required(VERSION 2.8)

project(FindLargestBoundingBox)
add_executable(FindLargestBoundingBox FindLargestBoundingBox.cxx ${source_files})
target_link_libraries(FindLargestBoundingBox ${ITK_LIBRARIES})
INSTALL(TARGETS FindLargestBoundingBox  RUNTIME DESTINATION .)
