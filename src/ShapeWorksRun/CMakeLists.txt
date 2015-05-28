ADD_EXECUTABLE(ShapeWorksRun ShapeWorksRun.cxx ShapeWorksRunApp.h ShapeWorksRunApp.txx)
#TARGET_LINK_LIBRARIES(ShapeWorksRun ITKParticleSystem Utilities ITKIO ITKNumerics ITKBasicFilters ITKCommon tinyxml)
TARGET_LINK_LIBRARIES(ShapeWorksRun ITKParticleSystem Utilities ${ITK_LIBRARIES} tinyxml)
INSTALL(TARGETS ShapeWorksRun   RUNTIME DESTINATION .)
