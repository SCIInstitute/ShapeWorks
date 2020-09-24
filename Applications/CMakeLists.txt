
CONFIGURE_FILE(
  ${CMAKE_CURRENT_SOURCE_DIR}/Configuration.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/Configuration.h
  @ONLY
  )

INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )

add_subdirectory(shapeworks)


