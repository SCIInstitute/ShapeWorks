set(SOURCES
  Project.cpp
  Subject.cpp
  Settings.cpp
  )

set(HEADERS
  Project.h
  Subject.h
  Settings.h
  )

add_library(Project STATIC
  ${SOURCES}
  ${HEADERS}
  )

target_link_libraries(Project
  xlnt::xlnt
  )

target_include_directories(Project PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>)

set_target_properties(Project PROPERTIES PUBLIC_HEADER
  "${HEADERS}")
install(TARGETS Project
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION lib
  LIBRARY DESTINATION lib
  PUBLIC_HEADER DESTINATION include
  )
