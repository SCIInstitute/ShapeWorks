cmake_minimum_required(VERSION 2.8)

project(CloseHoles)
add_executable(CloseHoles CloseHoles.cxx ${source_files})
target_link_libraries(CloseHoles ${ITK_LIBRARIES})
INSTALL(TARGETS CloseHoles  RUNTIME DESTINATION .)
