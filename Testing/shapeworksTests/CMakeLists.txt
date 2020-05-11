set(TEST_SRCS
  shapeworksTests.cpp
  )

add_executable(shapeworksTests
  ${TEST_SRCS}
  )

target_link_libraries(shapeworksTests
  ${ITK_LIBRARIES} ${VTK_LIBRARIES}
  GTest::Main
  )
# fixme: make this depend on Applications/shapeworks

add_test(NAME shapeworksTests COMMAND shapeworksTests)
