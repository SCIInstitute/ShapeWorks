FLTK_WRAP_UI(alignshapes alignshapesGUI.fl)

ADD_EXECUTABLE(alignshapes alignshapes.cxx alignshapesApp.cxx ${alignshapes_FLTK_UI_SRCS})
TARGET_LINK_LIBRARIES(alignshapes Utilities VtkUtilities ITKIO ITKNumerics ITKBasicFilters ITKCommon ITKParticleSystem params ITKVtkFltk vtkIO vtkWidgets vtkRendering vtkGraphics vtkImaging vtkFiltering vtkCommon vtkIO ${FLTK_LIBRARIES})
INSTALL(TARGETS alignshapes   RUNTIME DESTINATION .)
