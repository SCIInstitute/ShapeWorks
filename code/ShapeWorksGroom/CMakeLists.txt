ADD_EXECUTABLE(ShapeWorksGroom ShapeWorksGroom.cxx)
TARGET_LINK_LIBRARIES(ShapeWorksGroom ITKIO ITKNumerics ITKBasicFilters ITKCommon params)
#TARGET_LINK_LIBRARIES(ShapeWorksGroom ITKIO ITKNumerics ITKBasicFilters ITKCommon params vtkRendering vtkIO vtkHybrid)
INSTALL(TARGETS ShapeWorksGroom  RUNTIME DESTINATION .)

ADD_CUSTOM_COMMAND(TARGET ShapeWorksGroom 
  POST_BUILD 
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:ShapeWorksGroom> ${CMAKE_BINARY_DIR} VERBATIM)
