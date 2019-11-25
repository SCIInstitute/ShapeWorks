
set(TEST_SRCS
  OptimizeTests.cc
  )
set(TEST_HDRS
  OptimizeTests.h
  )

add_executable(OptimizeTests
  ${TEST_SRCS}
  )

target_link_libraries(OptimizeTests
  Optimize
  gtest_main)

add_test(NAME OptimizeTests COMMAND OptimizeTests)
