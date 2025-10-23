set(TEST_SRCS
  ProjectTests.cpp
  )

add_executable(ProjectTests
  ${TEST_SRCS}
  )

target_link_libraries(ProjectTests
  Testing ${ITK_LIBRARIES} ${VTK_LIBRARIES}
  Mesh Project Project Image
  )

add_test(NAME ProjectTests COMMAND ProjectTests)
