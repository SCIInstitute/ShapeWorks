set(TEST_SRCS
  ImageTests.cpp
  )

add_executable(ImageTests
  ${TEST_SRCS}
  )

target_link_libraries(ImageTests
  tinyxml Mesh vgl vgl_algo Image Common Optimize Utils trimesh2
  ${ITK_LIBRARIES} ${VTK_LIBRARIES}
  Testing
  )

add_test(NAME ImageTests COMMAND ImageTests)
