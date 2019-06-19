project(FastMarching)
add_executable(FastMarching FastMarching.cxx ${source_files})
target_link_libraries(FastMarching ${ITK_LIBRARIES})
INSTALL(TARGETS FastMarching  RUNTIME DESTINATION .)
