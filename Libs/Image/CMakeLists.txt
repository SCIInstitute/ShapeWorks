# Build
set(Image_sources
  Image.cpp
  ImageUtils.cpp)
add_library(Image STATIC
  ${Image_sources})
target_include_directories(Image PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>)

# Install
set(Image_headers
  Image.h
  ImageUtils.h
  )
set_target_properties(Image PROPERTIES PUBLIC_HEADER
  "${Image_headers}")
install(TARGETS Image
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION lib
  LIBRARY DESTINATION lib
  PUBLIC_HEADER DESTINATION include
  )

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
target_link_libraries(ExtractGivenLabelImage ${ITK_LIBRARIES} Utils Image)
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

# <ctc> gotta go!
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
