ADD_EXECUTABLE(TransformSplicer TransformSplicer.cxx)
TARGET_LINK_LIBRARIES(TransformSplicer Utilities ITKParticleSystem tinyxml ITKIO 
ITKNumerics ITKBasicFilters ITKCommon)

ADD_EXECUTABLE(ComputeModeLengths ComputeModeLengths.cxx)
TARGET_LINK_LIBRARIES(ComputeModeLengths Utilities ITKParticleSystem tinyxml ITKIO 
ITKNumerics ITKBasicFilters ITKCommon)

ADD_EXECUTABLE(GenerateScalingTransforms GenerateScalingTransforms.cxx)
TARGET_LINK_LIBRARIES(GenerateScalingTransforms Utilities ITKParticleSystem tinyxml ITKIO 
ITKNumerics ITKBasicFilters ITKCommon)

ADD_EXECUTABLE(FunctionOfPositionGenerator FunctionOfPositionGenerator.cxx)
TARGET_LINK_LIBRARIES(FunctionOfPositionGenerator Utilities ITKParticleSystem tinyxml ITKIO 
ITKNumerics ITKBasicFilters ITKCommon)

ADD_EXECUTABLE(AffineTransformer AffineTransformer.cxx)
TARGET_LINK_LIBRARIES(AffineTransformer Utilities ITKParticleSystem tinyxml ITKIO 
ITKNumerics ITKBasicFilters ITKCommon)

ADD_EXECUTABLE(ComputePointDifferences ComputePointDifferences.cxx)
TARGET_LINK_LIBRARIES(ComputePointDifferences Utilities ITKParticleSystem tinyxml ITKIO 
ITKNumerics ITKBasicFilters ITKCommon)

ADD_EXECUTABLE(TransformExtractor TransformExtractor.cxx)
TARGET_LINK_LIBRARIES(TransformExtractor Utilities ITKParticleSystem tinyxml ITKIO 
ITKNumerics ITKBasicFilters ITKCommon)

ADD_EXECUTABLE(MeanSceneAssembler MeanSceneAssembler.cxx)
TARGET_LINK_LIBRARIES(MeanSceneAssembler Utilities ITKParticleSystem tinyxml ITKIO 
ITKNumerics ITKBasicFilters ITKCommon)

ADD_EXECUTABLE(MeanPoser MeanPoser.cxx)
TARGET_LINK_LIBRARIES(MeanPoser Utilities ITKParticleSystem tinyxml ITKIO 
ITKNumerics ITKBasicFilters ITKCommon)

ADD_EXECUTABLE(ConvertTransformMatricesToTextQuaternions ConvertTransformMatricesToTextQuaternions.cxx)
TARGET_LINK_LIBRARIES(ConvertTransformMatricesToTextQuaternions Utilities ITKParticleSystem tinyxml ITKIO 
ITKNumerics ITKBasicFilters ITKCommon)

ADD_EXECUTABLE(ProcrustesAnalysis ProcrustesAnalysis.cxx)
TARGET_LINK_LIBRARIES(ProcrustesAnalysis Utilities ITKParticleSystem tinyxml ITKIO 
ITKNumerics ITKBasicFilters ITKCommon)

#ADD_EXECUTABLE(TPSAnalysis TPSAnalysis.cxx)
#TARGET_LINK_LIBRARIES(TPSAnalysis Utilities ITKParticleSystem tinyxml ITKIO 
#ITKNumerics ITKBasicFilters ITKCommon)

ADD_EXECUTABLE(ComputeMeanCorrespondences ComputeMeanCorrespondences.cxx)
TARGET_LINK_LIBRARIES(ComputeMeanCorrespondences Utilities ITKParticleSystem tinyxml ITKIO 
ITKNumerics ITKBasicFilters ITKCommon)

ADD_EXECUTABLE(ProcrustesAlign ProcrustesAlign.cxx)
TARGET_LINK_LIBRARIES(ProcrustesAlign Utilities ITKParticleSystem tinyxml ITKIO 
ITKNumerics ITKBasicFilters ITKCommon)

ADD_EXECUTABLE(ScalePoints ScalePoints.cxx)
TARGET_LINK_LIBRARIES(ScalePoints Utilities ITKParticleSystem tinyxml ITKIO 
ITKNumerics ITKBasicFilters ITKCommon)

ADD_EXECUTABLE(ApplyFixedTranslationToPointFile ApplyFixedTranslationToPointFile.cxx)
TARGET_LINK_LIBRARIES(ApplyFixedTranslationToPointFile Utilities ITKParticleSystem ITKIO ITKNumerics ITKBasicFilters ITKCommon)

ADD_EXECUTABLE(SegmentationVolumeCalculator SegmentationVolumeCalculator.cxx)
TARGET_LINK_LIBRARIES(SegmentationVolumeCalculator Utilities ITKParticleSystem tinyxml ITKIO 
ITKNumerics ITKBasicFilters ITKCommon)

ADD_EXECUTABLE(MetaSceneAssembler MetaSceneAssembler.cxx)
TARGET_LINK_LIBRARIES(MetaSceneAssembler Utilities ITKParticleSystem tinyxml ITKIO 
ITKNumerics ITKBasicFilters ITKCommon)

ADD_EXECUTABLE(PointsToMetaConverter PointsToMetaConverter.cxx)
TARGET_LINK_LIBRARIES(PointsToMetaConverter Utilities ITKIO ITKNumerics ITKBasicFilters ITKCommon)

ADD_EXECUTABLE(MeshToMetaConverter MeshToMetaConverter.cxx)
TARGET_LINK_LIBRARIES(MeshToMetaConverter Utilities ITKIO ITKNumerics ITKBasicFilters ITKCommon)
