cmake_minimum_required(VERSION 2.8)

project(PadVolumeWithConstant)
add_executable(PadVolumeWithConstant PadVolumeWithConstant.cxx )
target_link_libraries(PadVolumeWithConstant ${ITK_LIBRARIES} OptionParser)
INSTALL(TARGETS PadVolumeWithConstant RUNTIME DESTINATION bin)
