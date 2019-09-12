cmake_minimum_required(VERSION 2.8.12)
project(shapeworkspost)

add_executable(shapeworkspost shapeworkspost.cpp)

target_include_directories(shapeworkspost PRIVATE ${LIBIGL_INCLUDE_DIRS})
target_compile_definitions(shapeworkspost PRIVATE ${LIBIGL_DEFINITIONS})
target_link_libraries(shapeworkspost ${LIBIGL_LIBRARIES} ${LIBIGL_VIEWER_EXTRA_LIBRARIES} ${LIBIGL_TETGEN_EXTRA_LIBRARIES} ${ITK_LIBRARIES} tinyxml)

INSTALL(TARGETS shapeworkspost RUNTIME DESTINATION bin)