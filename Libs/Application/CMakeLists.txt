SET(APPLICATION_MOC_HDRS
  DeepSSM/DeepSSMJob.h
  Job/Job.h
  Job/PythonWorker.h
  ShapeWorksVtkOutputWindow.h
)

qt5_wrap_cpp( APPLICATION_MOC_SRCS ${APPLICATION_MOC_HDRS} )

SET(Application_headers
 )

add_library(Application STATIC
  DeepSSM/DeepSSMJob.cpp
  Job/Job.cpp
  Job/PythonWorker.cpp
  ShapeWorksVtkOutputWindow.cpp
  ${APPLICATION_MOC_SRCS}
  )

target_include_directories(Application PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>)

set(SW_PYTHON_LIBS pybind11::embed)

if (APPLE)
  include_directories(${_Python3_INCLUDE_DIR})
  set(SW_PYTHON_LIBS "")
endif(APPLE)

target_link_libraries(Application PUBLIC
  Groom
  Mesh
  Utils
  Particles
  Project
  ${SW_PYTHON_LIBS}
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
