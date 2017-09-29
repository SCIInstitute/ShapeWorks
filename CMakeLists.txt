#SET(RUN_NAME ShapeWorksRun2_withCotan)
#SET(RUN_NAME ShapeWorksRun2_weighSampling_wCorrespondence)
#SET(RUN_NAME ShapeWorksRun2_weighSampling_woCorrespondence)
#SET(RUN_NAME ShapeWorksRun2.1_weighSampling_wCorrespondence)
#SET(RUN_NAME ShapeWorksRun2.1_weighSampling_wCorrespondence_adaptiveSigma)
#SET(RUN_NAME ShapeWorksRun2_modifiedCotanWithFixedSigmaWithCorrespondence)
#SET(RUN_NAME ShapeWorksRun3_FeaMeshes)
# SHIREEN
#SET(RUN_NAME ShapeWorksRun3)
#SET(RUN_NAME ShapeWorksRun3.2) #fea meshes + normal penalty new code + bug fixes
#SET(RUN_NAME ShapeWorksRun3.5) #different number of particles per domain, different number of attributes per domain
#SET(RUN_NAME ShapeWorksRun4_debug)    # replace bad particles during optimization

SET(RUN_NAME ShapeWorksRun4.3) #allow different bounding box for every domain
#SET(RUN_NAME ShapeWorksRun4.4_debug) # enforce points to satisfy constraints, ability to use different number of cutting planes and spheres per domain, ability to specify in/out for sphere
ADD_EXECUTABLE(${RUN_NAME} ShapeWorksRun.cxx ShapeWorksRunApp.h ShapeWorksRunApp.txx)
#TARGET_LINK_LIBRARIES(ShapeWorksRun ITKParticleSystem Utilities ITKIO ITKNumerics ITKBasicFilters ITKCommon tinyxml)
TARGET_LINK_LIBRARIES(${RUN_NAME} ITKParticleSystem Utilities ${ITK_LIBRARIES} ${VTK_LIBRARIES} tinyxml fim_v4 vgl vgl_algo)

#TARGET_LINK_LIBRARIES(${RUN_NAME} ITKParticleSystem Utilities ${VTK_LIBRARIES} ITKIONRRD ITKIOMesh ITKIOBioRad ITKIOBMP ITKIOCSV ITKIOGDCM ITKIOGE ITKIOGIPL ITKIOHDF5 ITKIOImageBase ITKIOIPL ITKIOJPEG ITKIOLSM ITKIOMeta ITKIOMRC ITKIONIFTI ITKIOPNG ITKIOTIFF ITKIOStimulate ITKIOVTK  ITKCommon itkvcl itkvnl_algo itkvnl vgl_algo vgl itkv3p_netlib itkNetlibSlatec itkv3p_lsqr ITKVNLInstantiation itkzlib  itktestlib fim_v4 tinyxml)
INSTALL(TARGETS ${RUN_NAME}  RUNTIME DESTINATION .)
