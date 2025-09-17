set(TEST_SRCS
  UseCaseTests.cpp
  )

add_executable(UseCaseTests
  ${TEST_SRCS}
  )

target_link_libraries(UseCaseTests
  Testing Mesh Optimize Utils
  ${ITK_LIBRARIES} ${VTK_LIBRARIES}
  )

add_test(NAME UseCaseTests COMMAND UseCaseTests)
