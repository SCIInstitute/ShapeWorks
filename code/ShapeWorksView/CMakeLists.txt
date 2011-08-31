OPTION( BUILD_OSX_APP "Build ShapeWorksView as an application." OFF )

IF ( BUILD_OSX_APP )
	ADD_EXECUTABLE(ShapeWorksView MACOSX_BUNDLE ShapeWorksView.cxx ShapeWorksViewApp.cxx AnalyzeCorrespondenceGUI.cxx)
ELSE( BUILD_OSX_APP )
	ADD_EXECUTABLE(ShapeWorksView ShapeWorksView.cxx ShapeWorksViewApp.cxx AnalyzeCorrespondenceGUI.cxx)
ENDIF ( BUILD_OSX_APP )

IF(NOT APPLE)
FLTK_WRAP_UI(ShapeWorksView AnalyzeCorrespondenceGUI.fl)

TARGET_LINK_LIBRARIES(ShapeWorksView VtkUtilities Utilities ITKParticleSystem ITKIO ITKNumerics ITKBasicFilters ITKCommon params ITKVtkFltk vtkIO vtkWidgets vtkRendering vtkImaging vtkFiltering vtkHybrid vtkGraphics vtkCommon ${FLTK_LIBRARIES})

ELSE(NOT APPLE)

TARGET_LINK_LIBRARIES(ShapeWorksView VtkUtilities Utilities ITKParticleSystem ITKIO ITKNumerics ITKBasicFilters ITKCommon params ITKVtkFltk vtkIO vtkWidgets vtkRendering vtkImaging vtkFiltering vtkHybrid vtkGraphics vtkCommon ${FLTK_LIBRARIES})

ENDIF(NOT APPLE)

INSTALL(TARGETS ShapeWorksView   RUNTIME DESTINATION . BUNDLE DESTINATION . )

