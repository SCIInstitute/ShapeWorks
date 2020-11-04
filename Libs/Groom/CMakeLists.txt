
add_library(Groom STATIC
  Groom.cpp
  )

target_link_libraries(Groom
  Image
  Optimize
  Mesh
  Utils
  )

target_include_directories(Groom PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>
  )
