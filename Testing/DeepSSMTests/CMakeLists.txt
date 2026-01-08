set(TEST_SRCS
  DeepSSMTests.cpp
  )

add_executable(DeepSSMTests
  ${TEST_SRCS}
  )

target_link_libraries(DeepSSMTests
  Testing
  )

add_test(NAME DeepSSMTests COMMAND DeepSSMTests)
