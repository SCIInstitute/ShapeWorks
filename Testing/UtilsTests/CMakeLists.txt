set(TEST_SRCS
  UtilsTests.cpp
  )

add_executable(UtilsTests
  ${TEST_SRCS}
  )

target_link_libraries(UtilsTests
  Utils
  # Common
  # ${ITK_LIBRARIES}
  Testing
  )

add_test(NAME UtilsTests COMMAND UtilsTests)
