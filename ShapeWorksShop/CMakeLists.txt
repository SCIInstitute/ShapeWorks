FLTK_WRAP_UI(ShapeWorksShop CorrespondenceGUI.fl)

SET(SHAPEWORKS_SHOP_SRCS
  ShapeWorksShop.cxx
  ShapeWorksShopApp.cxx
  ShapeWorksShopApp.h
  CorrespondenceGUI.cxx
  ../Utilities/isosurface_pipeline.h
  ../Utilities/glyph_pipeline.h
  ../Utilities/sphere_widget_pipeline.h
  ../Utilities/plane_widget_pipeline.h
  )

IF(APPLE)
# Apple

ADD_EXECUTABLE(ShapeWorksShop ${SHAPEWORKS_SHOP_SRCS})
TARGET_LINK_LIBRARIES(ShapeWorksShop VtkUtilities Utilities ITKParticleSystem ITKIO ITKNumerics ITKBasicFilters ITKCommon tinyxml ITKVtkFltk vtkIO vtkWidgets vtkRendering vtkImaging vtkFiltering vtkHybrid vtkGraphics vtkCommon ${FLTK_LIBRARIES})

ELSE(APPLE)

  IF(WIN32)

  # Windows
    ADD_EXECUTABLE(ShapeWorksShop ${SHAPEWORKS_SHOP_SRCS} ${ShapeWorksShop_FLTK_UI_SRCS})
    TARGET_LINK_LIBRARIES(ShapeWorksShop VtkUtilities Utilities ITKParticleSystem ${ITK_LIBRARIES} tinyxml ITKVtkFltk vtkIO vtkWidgets vtkRendering vtkImaging vtkFiltering vtkHybrid vtkGraphics vtkCommon ${FLTK_LIBRARIES})

  ELSE(WIN32)

  # Linux
    ADD_EXECUTABLE(ShapeWorksShop ${SHAPEWORKS_SHOP_SRCS} ${ShapeWorksShop_FLTK_UI_SRCS})
    TARGET_LINK_LIBRARIES(ShapeWorksShop VtkUtilities Utilities ITKParticleSystem ${ITK_LIBRARIES} tinyxml ITKVtkFltk vtkIO vtkWidgets vtkRendering vtkImaging vtkFiltering vtkHybrid vtkGraphics vtkCommon ${FLTK_LIBRARIES} fontconfig Xinerama)

  ENDIF(WIN32)
ENDIF(APPLE)

INSTALL(TARGETS ShapeWorksShop   RUNTIME DESTINATION .)

