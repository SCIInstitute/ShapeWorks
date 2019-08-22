cmake_minimum_required(VERSION 2.8)

project(ReflectVolumes)
add_executable(ReflectVolumes ReflectVolumes.cxx)
target_link_libraries(ReflectVolumes ${ITK_LIBRARIES} OptionParser)
INSTALL(TARGETS ReflectVolumes  RUNTIME DESTINATION .)
