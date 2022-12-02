add_library(Groom STATIC
  Groom.cpp
  GroomParameters.cpp
  )

set(HEADERS
  Groom.h
  GroomParameters.h)

target_link_libraries(Groom
  Mesh
  Image
  Utils
  Boost::filesystem
  TBB::tbb
  )

set_target_properties(Groom PROPERTIES PUBLIC_HEADER
        "${HEADERS}")

target_include_directories(Groom PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>
  )

install(TARGETS Groom EXPORT ShapeWorksTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION include/Groom
        )
