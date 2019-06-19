cmake_minimum_required(VERSION 2.8)

project(ReflectVolumes)
add_executable(ReflectVolumes ReflectVolumes.cxx ${source_files})
target_link_libraries(ReflectVolumes ${ITK_LIBRARIES})
INSTALL(TARGETS ReflectVolumes  RUNTIME DESTINATION .)
