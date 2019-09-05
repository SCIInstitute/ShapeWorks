project(FastMarching)
add_executable(FastMarching FastMarching.cxx)
target_link_libraries(FastMarching ${ITK_LIBRARIES} OptionParser)
INSTALL(TARGETS FastMarching  RUNTIME DESTINATION bin)
