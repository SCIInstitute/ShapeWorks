set(SOURCES
  ExcelProjectReader.cpp
  ExcelProjectWriter.cpp
  JsonProjectReader.cpp
  JsonProjectWriter.cpp
  Parameters.cpp
  Project.cpp
  ProjectReader.cpp
  ProjectUtils.cpp
  Subject.cpp
  Variant.cpp
  )

set(HEADERS
  Project.h
  Subject.h
  Parameters.h
  ProjectUtils.h
  Variant.h
  )

add_library(Project STATIC
  ${SOURCES}
  ${HEADERS}
  )

target_link_libraries(Project
  xlnt::xlnt
  Mesh
  )

target_include_directories(Project PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>)

set_target_properties(Project PROPERTIES PUBLIC_HEADER
  "${HEADERS}")

install(TARGETS Project EXPORT ShapeWorksTargets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION include/Project
  )
