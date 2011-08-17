ADD_EXECUTABLE(ShapeWorksRun ShapeWorksRun.cxx)
TARGET_LINK_LIBRARIES(ShapeWorksRun ITKParticleSystem Utilities ITKIO ITKNumerics ITKBasicFilters ITKCommon tinyxml)
INSTALL(TARGETS ShapeWorksRun   RUNTIME DESTINATION .)
