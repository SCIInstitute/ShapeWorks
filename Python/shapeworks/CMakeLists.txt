find_package(pybind11 2.4.3 REQUIRED)

pybind11_add_module(shapeworkspy ShapeworksPython.cpp)

# ?s
# - how do we create nested modules? Maybe already works, just try the following:
#   `import shapeworkspy.Image as Image`
# - can we create a target simple named shapeworks so we can `import shapeworks` rather than shapeworkspy?

target_link_libraries(shapeworkspy PUBLIC
  Image
  Mesh
  ${ITK_LIBRARIES}
  )

  # tinyxml Mesh vgl vgl_algo Image Optimize Utils trimesh2
  # gtest_main ${ITK_LIBRARIES} ${VTK_LIBRARIES})

install(TARGETS shapeworkspy
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  PUBLIC_HEADER DESTINATION include
  )
