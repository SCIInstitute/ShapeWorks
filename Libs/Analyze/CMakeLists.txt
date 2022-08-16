
add_library(Analyze STATIC
  Analyze.cpp
  Particles.cpp
  Vis.cpp
  )

target_include_directories(Analyze PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>)

target_link_libraries(Analyze PUBLIC
  Mesh
  Utils)

# set
set_target_properties(Analyze PROPERTIES PUBLIC_HEADER
  "${Analyze_headers}")

install(TARGETS Analyze
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION lib
  LIBRARY DESTINATION lib
  PUBLIC_HEADER DESTINATION include
  )
