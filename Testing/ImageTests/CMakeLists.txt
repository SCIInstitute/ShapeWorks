

set(TEST_SRCS
  ImageTests.cc
  )

set(TEST_HDRS
  ImageTests.h
  )

add_executable(ImageTests
  ${TEST_SRCS}
  )

target_link_libraries(ImageTests
  tinyxml Mesh vgl vgl_algo Image Optimize Utils trimesh2
  gtest_main ${ITK_LIBRARIES} ${VTK_LIBRARIES})

add_test(NAME ImageTests COMMAND ImageTests)
