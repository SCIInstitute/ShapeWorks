find_package(pybind11 2.4.3 REQUIRED)

pybind11_add_module(shapeworkspy ShapeworksPython.cpp)

# ?s
# - how do we create nested modules?
# - how do we link necessary libraries? (Image, etc)

target_link_libraries(shapeworkspy PUBLIC
  Image
  Mesh
  ${ITK_LIBRARIES}
  )
  # tinyxml Mesh vgl vgl_algo Image Optimize Utils trimesh2
  # gtest_main ${ITK_LIBRARIES} ${VTK_LIBRARIES})
