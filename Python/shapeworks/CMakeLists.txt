find_package(pybind11 2.5.0 REQUIRED)

pybind11_add_module(shapeworks_py ShapeworksPython.cpp)

target_link_libraries(shapeworks_py PUBLIC
  Common
  Image
  Mesh
  Optimize
  Particles
  ${ITK_LIBRARIES}
  )

  # tinyxml  vgl vgl_algo  Utils trimesh2
  # gtest_main ${ITK_LIBRARIES} ${VTK_LIBRARIES})

set_target_properties(shapeworks_py PROPERTIES
  OUTPUT_NAME shapeworks
  )

install(TARGETS shapeworks_py
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  PUBLIC_HEADER DESTINATION include
  )
