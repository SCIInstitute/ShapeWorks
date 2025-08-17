SET(APPLICATION_MOC_HDRS
  Job/Job.h
)

qt5_wrap_cpp( APPLICATION_MOC_SRCS ${APPLICATION_MOC_HDRS} )

SET(Application_headers
 )

add_library(Application STATIC
  Job/Job.cpp
  ${APPLICATION_MOC_SRCS}
  )

target_include_directories(Application PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>)

target_link_libraries(Application PUBLIC
  Mesh
  Utils
  Particles
  )

# set
set_target_properties(Application PROPERTIES PUBLIC_HEADER
  "${Application_headers}")

install(TARGETS Application EXPORT ShapeWorksTargets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION include/Application
  )
