ADD_EXECUTABLE(ShapeWorksRun ShapeWorksRun.cxx)
#TARGET_LINK_LIBRARIES(ShapeWorksRun ITKParticleSystem Utilities ITKIO ITKNumerics ITKBasicFilters ITKCommon tinyxml)
TARGET_LINK_LIBRARIES(ShapeWorksRun ITKParticleSystem Utilities ${ITK_LIBRARIES} tinyxml)
INSTALL(TARGETS ShapeWorksRun   RUNTIME DESTINATION .)
