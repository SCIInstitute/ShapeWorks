FILE(GLOB trimesh2_sources ./libsrc/*.cc)
add_library(trimesh2 STATIC
  ${trimesh2_sources}
  )
target_include_directories(trimesh2 PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

# make it easier to see more important compiler warnings and errors (# TODO: fix the actual cause of the warnings)
if (UNIX)
  target_compile_options(trimesh2 PRIVATE -Wno-deprecated-declarations)
endif()
