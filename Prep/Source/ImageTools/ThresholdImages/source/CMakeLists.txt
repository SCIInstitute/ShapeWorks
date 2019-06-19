cmake_minimum_required(VERSION 2.8)

PROJECT(ThresholdImages)
add_executable(ThresholdImages ThresholdImages.cxx ${source_files})
target_link_libraries(ThresholdImages ${ITK_LIBRARIES})
INSTALL(TARGETS ThresholdImages  RUNTIME DESTINATION .)