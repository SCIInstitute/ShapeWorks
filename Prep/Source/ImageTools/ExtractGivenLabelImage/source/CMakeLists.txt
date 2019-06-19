cmake_minimum_required(VERSION 2.8)

project(ExtractGivenLabelImage)
add_executable(ExtractGivenLabelImage ExtractGivenLabelImage.cxx ${source_files})
target_link_libraries(ExtractGivenLabelImage ${ITK_LIBRARIES})
INSTALL(TARGETS ExtractGivenLabelImage  RUNTIME DESTINATION .)

