ADD_EXECUTABLE(ShapeWorksGroom ShapeWorksGroom.cxx)
TARGET_LINK_LIBRARIES(ShapeWorksGroom ITKIO ITKNumerics ITKBasicFilters ITKCommon params)
#TARGET_LINK_LIBRARIES(ShapeWorksGroom ITKIO ITKNumerics ITKBasicFilters ITKCommon params vtkRendering vtkIO vtkHybrid)
INSTALL(TARGETS ShapeWorksGroom  RUNTIME DESTINATION .)
