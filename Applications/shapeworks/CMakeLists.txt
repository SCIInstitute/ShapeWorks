# Build
set(shapeworks_sources
  shapeworks.cpp
  Executable.cpp
  Command.cpp
  Commands.cpp
  )
set(shapeworks_headers
  Executable.h
  Command.h
  Commands.h
  SharedCommandData.h
  )
add_executable(shapeworks_exe
  ${shapeworks_sources}
  ${shapeworks_headers})
target_include_directories(shapeworks_exe PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>)
target_link_libraries(shapeworks_exe
  Utils
  Image
  Common
  Mesh
  Analyze
  Particles
  ${ITK_LIBRARIES}
  )

# Install
set_target_properties(shapeworks_exe PROPERTIES
  PUBLIC_HEADER "${shapeworks_headers}"
  OUTPUT_NAME shapeworks
  #ENABLE_EXPORTS TRUE  # needed to enable dependency of shapeworksTests (fixme: doesn't work with gcc, but works with Xcode)
  # https://gitlab.kitware.com/cmake/cmake/-/issues/19527
  )
install(TARGETS shapeworks_exe
  ARCHIVE DESTINATION bin
  RUNTIME DESTINATION bin
  PUBLIC_HEADER DESTINATION include
  )
