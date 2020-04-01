find_package(pybind11 2.4.3 REQUIRED)

pybind11_add_module(shapeworks ShapeworksPython.cpp)

target_link_libraries(shapeworks PUBLIC
  Common
  Image
  Mesh
  Optimize
  Particles
  ${ITK_LIBRARIES}
  )

  # tinyxml  vgl vgl_algo  Utils trimesh2
  # gtest_main ${ITK_LIBRARIES} ${VTK_LIBRARIES})

install(TARGETS shapeworks
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  PUBLIC_HEADER DESTINATION include
  )
