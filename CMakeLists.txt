#SET(RUN_NAME ShapeWorksRun2_withCotan)
#SET(RUN_NAME ShapeWorksRun2_weighSampling_wCorrespondence)
#SET(RUN_NAME ShapeWorksRun2_weighSampling_woCorrespondence)
#SET(RUN_NAME ShapeWorksRun2.1_weighSampling_wCorrespondence)
#SET(RUN_NAME ShapeWorksRun2.1_weighSampling_wCorrespondence_adaptiveSigma)
#SET(RUN_NAME ShapeWorksRun2_modifiedCotanWithFixedSigmaWithCorrespondence)
#SET(RUN_NAME ShapeWorksRun3_FeaMeshes)
SET(RUN_NAME ShapeWorksRun3.2)

# SHIREEN
SET(RUN_NAME ShapeWorksRun3)

ADD_EXECUTABLE(${RUN_NAME} ShapeWorksRun.cxx ShapeWorksRunApp.h ShapeWorksRunApp.txx)
#TARGET_LINK_LIBRARIES(ShapeWorksRun ITKParticleSystem Utilities ITKIO ITKNumerics ITKBasicFilters ITKCommon tinyxml)
TARGET_LINK_LIBRARIES(${RUN_NAME} ITKParticleSystem Utilities ${ITK_LIBRARIES} ${VTK_LIBRARIES} tinyxml fim_v4 vgl vgl_algo)

#TARGET_LINK_LIBRARIES(${RUN_NAME} ITKParticleSystem Utilities ${VTK_LIBRARIES} ITKIONRRD ITKIOMesh ITKIOBioRad ITKIOBMP ITKIOCSV ITKIOGDCM ITKIOGE ITKIOGIPL ITKIOHDF5 ITKIOImageBase ITKIOIPL ITKIOJPEG ITKIOLSM ITKIOMeta ITKIOMRC ITKIONIFTI ITKIOPNG ITKIOTIFF ITKIOStimulate ITKIOVTK  ITKCommon itkvcl itkvnl_algo itkvnl vgl_algo vgl itkv3p_netlib itkNetlibSlatec itkv3p_lsqr ITKVNLInstantiation itkzlib  itktestlib fim_v4 tinyxml)
INSTALL(TARGETS ${RUN_NAME}   RUNTIME DESTINATION .)
