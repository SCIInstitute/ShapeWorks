cmake_minimum_required(VERSION 2.8)

PROJECT(ThresholdImages)
add_executable(ThresholdImages ThresholdImages.cxx)
target_link_libraries(ThresholdImages ${ITK_LIBRARIES} OptionParser)
INSTALL(TARGETS ThresholdImages  RUNTIME DESTINATION .)
