FILE(GLOB tinyxml_srcs ./*.cpp)
if(TARGET tinyxml)
    message("tinyxml already exists")
else(TARGET tinyxml)
    ADD_LIBRARY (tinyxml STATIC ${tinyxml_srcs})
#    target_include_directories(tinyxml PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<INSTALL_INTERFACE:include)
endif(TARGET tinyxml)

install(TARGETS tinyxml EXPORT ShapeWorksTargets
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION lib
  LIBRARY DESTINATION lib
  PUBLIC_HEADER DESTINATION include
  )
