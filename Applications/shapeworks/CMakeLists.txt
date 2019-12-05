# Build
set(shapeworks_sources
  main.cpp Executable.cpp Command.cpp Commands.cpp)
add_executable(shapeworks
  ${shapeworks_sources})
target_include_directories(shapeworks PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>)
target_link_libraries(shapeworks
  ${ITK_LIBRARIES}
  Utils
  Image
  )

# Install
set(shapeworks_headers
  Executable.h Command.h Commands.h)
set_target_properties(shapeworks PROPERTIES PUBLIC_HEADER
  "${shapeworks_headers}")
install(TARGETS shapeworks
  ARCHIVE DESTINATION bin
  RUNTIME DESTINATION bin
  PUBLIC_HEADER DESTINATION include
  )



  message(STATUS "include directories...") # because -isystem /path/to/shapeworks/install/include is there no matter what the build type, argh!
  #get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
  # GLOBAL,  DIRECTORY, TARGET, SOURCE, TEST, VARIABLE, CACHE, INSTALL
  # get_property(dirs TEST shapeworks (doesn't exist) PROPERTY INCLUDE_DIRECTORIES)
  #get_property(dirs SOURCE ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
  #get_property(dirs GLOBAL PROPERTY INCLUDE_DIRECTORIES)
  #get_property(dirs TARGET shapeworks PROPERTY INCLUDE_DIRECTORIES)
  # get_property(dirs CACHE <scope?> PROPERTY INCLUDE_DIRECTORIES)
  #get_property(dirs INSTALL shapeworks PROPERTY INCLUDE_DIRECTORIES)  (nothing, even when install_interface specified above)
  foreach(dir ${dirs})
    message(STATUS "dir='${dir}'")
  endforeach()
