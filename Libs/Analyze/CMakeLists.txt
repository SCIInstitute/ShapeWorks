# Build
set(Vis_sources
  Vis.cpp
  ShapeCollection.cpp)
set(Vis_headers
  Vis.h
  ShapeCollection.h)
add_library(Analyze STATIC
  ${Vis_sources}
  ${Vis_headers})
target_include_directories(Analyze PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>)
target_link_libraries(Analyze PUBLIC
  Mesh
  Alignment
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

ADD_EXECUTABLE(MeshFromDT MeshFromDT.cpp)
TARGET_LINK_LIBRARIES(MeshFromDT ${VTK_LIBRARIES} ${ITK_LIBRARIES} Analyze tinyxml Utils)
INSTALL(TARGETS MeshFromDT RUNTIME DESTINATION bin)
