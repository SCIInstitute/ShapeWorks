FILE(GLOB trimesh2_sources ./libsrc/*.cc include.*.h)
FILE(GLOB trimesh2_headers ./include/*.h)

add_library(trimesh2 STATIC
  ${trimesh2_sources}
  )
target_include_directories(trimesh2 PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

set_target_properties(trimesh2 PROPERTIES PUBLIC_HEADER "${trimesh2_headers}")

# make it easier to see more important compiler warnings and errors (# TODO: fix the actual cause of the warnings)
if (UNIX)
  target_compile_options(trimesh2 PRIVATE -Wno-deprecated-declarations)
endif()

install(TARGETS trimesh2 EXPORT ShapeWorksTargets
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION lib
  LIBRARY DESTINATION lib
  PUBLIC_HEADER DESTINATION include
  )
