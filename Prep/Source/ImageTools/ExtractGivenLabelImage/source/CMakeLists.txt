cmake_minimum_required(VERSION 2.8)

project(ExtractGivenLabelImage)
add_executable(ExtractGivenLabelImage ExtractGivenLabelImage.cxx)
target_link_libraries(ExtractGivenLabelImage ${ITK_LIBRARIES} OptionParser)
INSTALL(TARGETS ExtractGivenLabelImage  RUNTIME DESTINATION bin)

