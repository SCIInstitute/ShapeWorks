INCLUDE_DIRECTORIES(${SCI_BOOST_INCLUDE})

# Set some Win32 Specific Settings
IF(WIN32)
	SET(GUI_TYPE WIN32)
	FLTK_WRAP_UI(ShapeWorksView AnalyzeCorrespondenceGUI.fl)
ENDIF(WIN32)
# Set some Apple MacOS Specific settings
IF (APPLE)
	SET(GUI_TYPE MACOSX_BUNDLE)
ENDIF (APPLE)

ADD_EXECUTABLE(ShapeWorksView ${GUI_TYPE} ShapeWorksView.cxx ShapeWorksViewApp.cxx AnalyzeCorrespondenceGUI.cxx)
#ADD_EXECUTABLE(ShapeWorksView  ShapeWorksView.cxx ShapeWorksViewApp.cxx AnalyzeCorrespondenceGUI.cxx)
TARGET_LINK_LIBRARIES(ShapeWorksView VtkUtilities 
	Utilities ITKParticleSystem ITKIO ITKNumerics
	ITKBasicFilters ITKCommon params 
	ITKVtkFltk vtkIO vtkWidgets vtkRendering 
	vtkImaging vtkFiltering vtkHybrid vtkGraphics 
	vtkCommon ${FLTK_LIBRARIES} 
#	${SCI_BOOST_LIBRARY}
	)

IF(WIN32)	
	INSTALL(TARGETS ShapeWorksView   RUNTIME DESTINATION .)
ENDIF(WIN32)
