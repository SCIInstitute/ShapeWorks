set(TEST_SRCS
  GroomTests.cpp
  )

add_executable(GroomTests
  ${TEST_SRCS}
  )

target_link_libraries(GroomTests
  Testing ${ITK_LIBRARIES} ${VTK_LIBRARIES}
  Mesh Groom Project Image
  )

add_test(NAME GroomTests COMMAND GroomTests)
