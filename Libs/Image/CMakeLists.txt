# Build
set(Image_sources
  Image.cpp
  Transform.cpp
  )
set(Image_headers
  Image.h
  Transform.h
  )
add_library(Image STATIC
  ${Image_sources}
  ${Image_headers})
target_include_directories(Image PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>)

# Install
set_target_properties(Image PROPERTIES PUBLIC_HEADER
  "${Image_headers}")
install(TARGETS Image
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION lib
  LIBRARY DESTINATION lib
  PUBLIC_HEADER DESTINATION include
  )

add_executable(ClipVolume ClipVolume.cxx)
target_link_libraries(ClipVolume ${ITK_LIBRARIES} ${VTK_LIBRARIES} tinyxml)
INSTALL(TARGETS ClipVolume RUNTIME DESTINATION bin)

add_executable(WriteImageInfoToText WriteImageInfoToText.cxx )
target_link_libraries(WriteImageInfoToText ${ITK_LIBRARIES} Utils)
INSTALL(TARGETS WriteImageInfoToText RUNTIME DESTINATION bin)

add_executable(FindLargestBoundingBox FindLargestBoundingBox.cxx )
target_link_libraries(FindLargestBoundingBox ${ITK_LIBRARIES} Utils)
INSTALL(TARGETS FindLargestBoundingBox  RUNTIME DESTINATION bin)

add_executable(ReflectVolumes ReflectVolumes.cxx)
target_link_libraries(ReflectVolumes ${ITK_LIBRARIES} Utils)
INSTALL(TARGETS ReflectVolumes  RUNTIME DESTINATION bin)

add_executable(MeshFromDistanceTransforms MeshFromDistanceTransforms.cxx)
target_link_libraries(MeshFromDistanceTransforms ${ITK_LIBRARIES} ${VTK_LIBRARIES} tinyxml)
INSTALL(TARGETS MeshFromDistanceTransforms  RUNTIME DESTINATION bin)
