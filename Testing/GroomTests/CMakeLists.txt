set(TEST_SRCS
  GroomTests.cpp
  )

add_executable(GroomTests
  ${TEST_SRCS}
  )

target_link_libraries(GroomTests
  ${ITK_LIBRARIES} ${VTK_LIBRARIES}
  Mesh Groom Project Image
  Testing
  )

add_test(NAME GroomTests COMMAND GroomTests)
