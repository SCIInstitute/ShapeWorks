# Build
set(Image_sources
  Image.cpp
  ImageUtils.cpp
  )
set(Image_headers
  Image.h
  ImageUtils.h
  )
add_library(Image STATIC
  ${Image_sources}
  ${Image_headers})
target_include_directories(Image PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>)
target_link_libraries(Image
  Common
  Mesh
  ${ITK_LIBRARIES}
  )


# Install
set_target_properties(Image PROPERTIES PUBLIC_HEADER
  "${Image_headers}")
install(TARGETS Image
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION lib
  LIBRARY DESTINATION lib
  PUBLIC_HEADER DESTINATION include
  )

add_executable(MeshFromDistanceTransforms MeshFromDistanceTransforms.cxx)
target_link_libraries(MeshFromDistanceTransforms ${ITK_LIBRARIES} ${VTK_LIBRARIES} tinyxml)
INSTALL(TARGETS MeshFromDistanceTransforms  RUNTIME DESTINATION bin)
