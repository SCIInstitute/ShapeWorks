FLTK_WRAP_UI(ShapeWorksShop CorrespondenceGUI.fl)

IF(NOT APPLE)

ADD_EXECUTABLE(ShapeWorksShop ShapeWorksShop.cxx ShapeWorksShopApp.cxx ${ShapeWorksShop_FLTK_UI_SRCS})

#TARGET_LINK_LIBRARIES(ShapeWorksShop VtkUtilities Utilities ITKParticleSystem ITKIO ITKNumerics ITKBasicFilters ITKCommon tinyxml ITKVtkFltk vtkIO vtkWidgets vtkRendering vtkImaging vtkFiltering vtkHybrid vtkGraphics vtkCommon ${FLTK_LIBRARIES})

TARGET_LINK_LIBRARIES(ShapeWorksShop VtkUtilities Utilities ITKParticleSystem ${ITK_LIBRARIES} tinyxml ITKVtkFltk vtkIO vtkWidgets vtkRendering vtkImaging vtkFiltering vtkHybrid vtkGraphics vtkCommon ${FLTK_LIBRARIES})

ELSE(NOT APPLE)

ADD_EXECUTABLE(ShapeWorksShop ShapeWorksShop.cxx ShapeWorksShopApp.cxx CorrespondenceGUI.cxx)
TARGET_LINK_LIBRARIES(ShapeWorksShop VtkUtilities Utilities ITKParticleSystem ITKIO ITKNumerics ITKBasicFilters ITKCommon tinyxml ITKVtkFltk vtkIO vtkWidgets vtkRendering vtkImaging vtkFiltering vtkHybrid vtkGraphics vtkCommon ${FLTK_LIBRARIES})

ENDIF(NOT APPLE)

INSTALL(TARGETS ShapeWorksShop   RUNTIME DESTINATION .)

