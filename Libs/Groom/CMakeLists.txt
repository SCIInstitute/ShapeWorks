add_library(Groom STATIC
  Groom.cpp
  GroomParameters.cpp
  )

target_link_libraries(Groom
  Image
  Mesh
  TBB::tbb
  Utils
  )

target_include_directories(Groom PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>
  )
