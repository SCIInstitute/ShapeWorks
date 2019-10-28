add_executable(PadVolumeWithConstant PadVolumeWithConstant.cxx )
target_link_libraries(PadVolumeWithConstant ${ITK_LIBRARIES} Utils)
INSTALL(TARGETS PadVolumeWithConstant RUNTIME DESTINATION bin)

add_executable(ClipVolume ClipVolume.cxx)
target_link_libraries(ClipVolume ${ITK_LIBRARIES} ${VTK_LIBRARIES} tinyxml)
INSTALL(TARGETS ClipVolume RUNTIME DESTINATION bin)

add_executable(CloseHoles CloseHoles.cxx )
target_link_libraries(CloseHoles ${ITK_LIBRARIES} Utils)
INSTALL(TARGETS CloseHoles RUNTIME DESTINATION bin)

add_executable(WriteImageInfoToText WriteImageInfoToText.cxx )
target_link_libraries(WriteImageInfoToText ${ITK_LIBRARIES} Utils)
INSTALL(TARGETS WriteImageInfoToText RUNTIME DESTINATION bin)

add_executable(ResampleVolumesToBeIsotropic ResampleVolumesToBeIsotropic.cxx)
target_link_libraries(ResampleVolumesToBeIsotropic ${ITK_LIBRARIES} Utils)
INSTALL(TARGETS ResampleVolumesToBeIsotropic RUNTIME DESTINATION bin)

add_executable(ExtractGivenLabelImage ExtractGivenLabelImage.cxx)
target_link_libraries(ExtractGivenLabelImage ${ITK_LIBRARIES} Utils)
INSTALL(TARGETS ExtractGivenLabelImage  RUNTIME DESTINATION bin)

add_executable(FastMarching FastMarching.cxx)
target_link_libraries(FastMarching ${ITK_LIBRARIES} Utils)
INSTALL(TARGETS FastMarching  RUNTIME DESTINATION bin)

ADD_EXECUTABLE(TopologyPreservingSmoothing TopologyPreservingSmoothing.cxx)
TARGET_LINK_LIBRARIES(TopologyPreservingSmoothing  tinyxml ${ITK_LIBRARIES})
INSTALL(TARGETS TopologyPreservingSmoothing  RUNTIME DESTINATION bin)

add_executable(CropImages CropImages.cxx)
target_link_libraries(CropImages ${ITK_LIBRARIES} Utils)
INSTALL(TARGETS CropImages  RUNTIME DESTINATION bin)

add_executable(AntiAliasing AntiAliasing.cxx)
target_link_libraries(AntiAliasing ${ITK_LIBRARIES} Utils)
INSTALL(TARGETS AntiAliasing  RUNTIME DESTINATION bin)

add_executable(ThresholdImages ThresholdImages.cxx)
target_link_libraries(ThresholdImages ${ITK_LIBRARIES} Utils)
INSTALL(TARGETS ThresholdImages  RUNTIME DESTINATION bin)

add_executable(FindLargestBoundingBox FindLargestBoundingBox.cxx )
target_link_libraries(FindLargestBoundingBox ${ITK_LIBRARIES} Utils)
INSTALL(TARGETS FindLargestBoundingBox  RUNTIME DESTINATION bin)

add_executable(ReflectVolumes ReflectVolumes.cxx)
target_link_libraries(ReflectVolumes ${ITK_LIBRARIES} Utils)
INSTALL(TARGETS ReflectVolumes  RUNTIME DESTINATION bin)
