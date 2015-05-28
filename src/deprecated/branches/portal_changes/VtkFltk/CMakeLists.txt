# This directory only builds if FLTK and VTK can be found
INCLUDE_DIRECTORIES(
  ${SHAPEWORKS_SOURCE_DIR}/VtkFltk
  ${SHAPEWORKS_BINARY_DIR}/VtkFltk
)

FLTK_WRAP_UI(ITKVtkFltk fltkVTKImageViewerGUI.fl)

ADD_LIBRARY(ITKVtkFltk
  vtkFlRenderWindowInteractor.cxx
  fltkVTKImageViewerBase.cxx
  ${ITKVtkFltk_FLTK_UI_SRCS}
)

TARGET_LINK_LIBRARIES(ITKVtkFltk vtkRendering ${FLTK_LIBRARIES} ${OPENGL_glu_LIBRARY})

IF(VTK_USE_VOLUMERENDERING)
  TARGET_LINK_LIBRARIES(ITKVtkFltk vtkVolumeRendering)
ENDIF(VTK_USE_VOLUMERENDERING)
