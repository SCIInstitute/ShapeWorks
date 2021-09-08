# Build
set(Common_sources
  Shapeworks.cpp
  ShapeworksUtils.cpp
  Region.cpp
  )
set(Common_headers
  Shapeworks.h
  ShapeworksUtils.h
  Region.h
  )
add_library(Common STATIC
  ${Common_sources}
  ${Common_headers})
target_include_directories(Common PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>)
target_link_libraries(Common
  ${ITK_LIBRARIES}
  ${VTK_LIBRARIES}
  )

# Install
set_target_properties(Common PROPERTIES PUBLIC_HEADER
  "${Common_headers}")
install(TARGETS Common
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION lib
  LIBRARY DESTINATION lib
  PUBLIC_HEADER DESTINATION include
  )
