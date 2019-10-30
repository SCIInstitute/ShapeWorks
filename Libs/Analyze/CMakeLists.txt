set(Vis_sources Vis.cpp)

add_library(Analyze STATIC
  ${Vis_sources})
target_include_directories(Analyze PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>)
target_link_libraries(Analyze PUBLIC
  Mesh
  Alignment
  Utils)

ADD_EXECUTABLE(MeshFromDT MeshFromDT.cpp)
TARGET_LINK_LIBRARIES(MeshFromDT ${VTK_LIBRARIES} ${ITK_LIBRARIES} Analyze tinyxml Utils)
INSTALL(TARGETS MeshFromDT RUNTIME DESTINATION bin)
