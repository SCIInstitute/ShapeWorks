# Build
find_package(Eigen3 3.3 REQUIRED NO_MODULE)

set(PointCloud_sources
  RBFShape.cpp
  )
set(PointCloud_headers
  RBFShape.h
  )
add_library(PointCloud STATIC
  ${PointCloud_sources}
  ${PointCloud_headers})
target_include_directories(PointCloud PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>)

# Install
set_target_properties(PointCloud PROPERTIES PUBLIC_HEADER
  "${PointCloud_headers}")
install(TARGETS PointCloud
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION lib
  LIBRARY DESTINATION lib
  PUBLIC_HEADER DESTINATION include
  )

target_link_libraries (PointCloud Eigen3::Eigen)
