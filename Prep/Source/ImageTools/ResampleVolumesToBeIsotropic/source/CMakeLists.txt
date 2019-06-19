cmake_minimum_required(VERSION 2.8)

project(ResampleVolumesToBeIsotropic)
add_executable(ResampleVolumesToBeIsotropic ResampleVolumesToBeIsotropic.cxx ${source_files})
target_link_libraries(ResampleVolumesToBeIsotropic ${ITK_LIBRARIES})
INSTALL(TARGETS ResampleVolumesToBeIsotropic  RUNTIME DESTINATION .)
