set(TEST_SRCS
  ProjectTests.cpp
  )

add_executable(ProjectTests
  ${TEST_SRCS}
  )

target_link_libraries(ProjectTests
  ${ITK_LIBRARIES} ${VTK_LIBRARIES}
  Mesh Project Project Image
  Testing
  )

add_test(NAME ProjectTests COMMAND ProjectTests)
