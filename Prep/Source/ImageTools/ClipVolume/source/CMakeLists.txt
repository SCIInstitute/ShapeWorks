cmake_minimum_required(VERSION 2.8)
PROJECT(ClipVolume)
add_executable(ClipVolume ClipVolume.cxx)
target_link_libraries(ClipVolume ${ITK_LIBRARIES} ${VTK_LIBRARIES} tinyxml)