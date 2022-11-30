set(TEST_SRCS
  UseCaseTests.cpp
  )

add_executable(UseCaseTests
  ${TEST_SRCS}
  )

target_link_libraries(UseCaseTests
  ${ITK_LIBRARIES} ${VTK_LIBRARIES}
  Testing
  )

add_test(NAME UseCaseTests COMMAND UseCaseTests)
