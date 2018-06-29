cmake_minimum_required(VERSION 2.8)

project(PadVolumeWithConstant)
add_executable(PadVolumeWithConstant PadVolumeWithConstant.cxx ${source_files})
target_link_libraries(PadVolumeWithConstant ${ITK_LIBRARIES})
INSTALL(TARGETS PadVolumeWithConstant  RUNTIME DESTINATION .)
