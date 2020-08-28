set(TEST_SRCS
  ImageTests.cpp
  )

add_executable(ImageTests
  ${TEST_SRCS}
  )

target_link_libraries(ImageTests
  Image
  Common
  ${ITK_LIBRARIES}
  Testing
  )

add_test(NAME ImageTests COMMAND ImageTests)
