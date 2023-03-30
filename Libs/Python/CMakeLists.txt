pybind11_add_module(shapeworks_py
  ShapeworksPython.cpp
  PythonAnalyze.cpp
  )

if(LINUX)
  SET(DTAG_EXTRA_FLAGS "-Wl,--disable-new-dtags")
endif(LINUX)

target_link_libraries(shapeworks_py PUBLIC
  ${VTK_LIBRARIES}
  Common
  Image
  Mesh
  Optimize
  Project
  Particles
  Project
  Analyze
  ${ITK_LIBRARIES}
  Eigen3::Eigen
  ${DTAG_EXTRA_FLAGS}
  )

set_target_properties(shapeworks_py PROPERTIES
  OUTPUT_NAME shapeworks_py
  )

install(TARGETS shapeworks_py
  ARCHIVE DESTINATION bin
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION bin
  PUBLIC_HEADER DESTINATION include
  )
