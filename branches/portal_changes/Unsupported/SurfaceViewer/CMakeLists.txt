FLTK_WRAP_UI(surfaceviewer surfaceviewerGUI.fl)

ADD_EXECUTABLE(surfaceviewer surfaceviewer.cxx surfaceviewerApp.cxx ${surfaceviewer_FLTK_UI_SRCS})
TARGET_LINK_LIBRARIES(surfaceviewer Utilities VtkUtilities ITKIO ITKNumerics ITKBasicFilters ITKCommon tinyxml vtkIO ITKVtkFltk vtkWidgets vtkRendering vtkGraphics vtkImaging vtkFiltering vtkCommon vtkIO ${FLTK_LIBRARIES})
INSTALL(TARGETS surfaceviewer   RUNTIME DESTINATION .)
