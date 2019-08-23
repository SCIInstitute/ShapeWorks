ADD_EXECUTABLE(psmCommandLineTool psmCommandLineTool.cxx)
TARGET_LINK_LIBRARIES(psmCommandLineTool ITKIOMeta ITKIOGDCM ITKIONRRD ITKParticleShapeModeling ITKIOXML  ${ITKIOImageBase_LIBRARIES} ${ITKstatistics_LIBRARIES} ${ITKTransform_LIBRARIES})
