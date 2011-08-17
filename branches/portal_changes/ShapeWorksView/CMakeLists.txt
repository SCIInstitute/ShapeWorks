IF(NOT APPLE)
FLTK_WRAP_UI(ShapeWorksView AnalyzeCorrespondenceGUI.fl)



ADD_EXECUTABLE(ShapeWorksView ShapeWorksView.cxx ShapeWorksViewApp.cxx ${ShapeWorksView_FLTK_UI_SRCS})

TARGET_LINK_LIBRARIES(ShapeWorksView VtkUtilities Utilities ITKParticleSystem ITKIO ITKNumerics ITKBasicFilters ITKCommon tinyxml ITKVtkFltk vtkIO vtkWidgets vtkRendering vtkImaging vtkFiltering vtkHybrid vtkGraphics vtkCommon ${FLTK_LIBRARIES})

ELSE(NOT APPLE)
ADD_EXECUTABLE(ShapeWorksView ShapeWorksView.cxx ShapeWorksViewApp.cxx AnalyzeCorrespondenceGUI.cxx)
TARGET_LINK_LIBRARIES(ShapeWorksView VtkUtilities Utilities ITKParticleSystem ITKIO ITKNumerics ITKBasicFilters ITKCommon tinyxml ITKVtkFltk vtkIO vtkWidgets vtkRendering vtkImaging vtkFiltering vtkHybrid vtkGraphics vtkCommon ${FLTK_LIBRARIES})

ENDIF(NOT APPLE)

INSTALL(TARGETS ShapeWorksView   RUNTIME DESTINATION .)

