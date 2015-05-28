FILE(GLOB SRCS "*.txx" "*.h" "*.cxx")


ADD_EXECUTABLE(ShapeWorksGroom ${SRCS})
#TARGET_LINK_LIBRARIES(ShapeWorksGroom ITKIO ITKNumerics ITKBasicFilters ITKCommon tinyxml)
TARGET_LINK_LIBRARIES(ShapeWorksGroom ${ITK_LIBRARIES} tinyxml)
#TARGET_LINK_LIBRARIES(ShapeWorksGroom ITKIO ITKNumerics ITKBasicFilters ITKCommon params tinyxml vtkRendering vtkIO vtkHybrid)
INSTALL(TARGETS ShapeWorksGroom  RUNTIME DESTINATION .)
