set(TEST_SRCS
  ImageTests.cpp
  )

add_executable(ImageTests
  ${TEST_SRCS}
  )

add_dependencies(ImageTests gtest gtest_main)


target_link_libraries(ImageTests
  Testing
  Image
  Common
  ${ITK_LIBRARIES}
  Project
  )

add_test(NAME ImageTests COMMAND ImageTests)
