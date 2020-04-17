# Build
set(Common_sources
  Shapeworks.cpp
  ShapeworksUtils.cpp
  )
set(Common_headers
  Shapeworks.h
  ShapeworksUtils.h
  )
add_library(Common STATIC
  ${Common_sources}
  ${Common_headers})
target_include_directories(Common PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>)

# Install
set_target_properties(Common PROPERTIES PUBLIC_HEADER
  "${Common_headers}")
install(TARGETS Common
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION lib
  LIBRARY DESTINATION lib
  PUBLIC_HEADER DESTINATION include
  )
