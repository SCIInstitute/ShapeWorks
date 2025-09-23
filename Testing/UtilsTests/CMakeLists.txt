set(TEST_SRCS
  UtilsTests.cpp
  )

add_executable(UtilsTests
  ${TEST_SRCS}
  )

target_link_libraries(UtilsTests
  Testing
  Utils
  )

add_test(NAME UtilsTests COMMAND UtilsTests)
