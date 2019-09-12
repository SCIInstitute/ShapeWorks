project(Resize_origin_resampleShapes)
cmake_minimum_required(VERSION 2.8)

ADD_EXECUTABLE(ResizeOriginResampleVolumes ResizeOriginResampleVolumes.cxx )
TARGET_LINK_LIBRARIES(ResizeOriginResampleVolumes ${ITK_LIBRARIES} tinyxml)
INSTALL(TARGETS ResizeOriginResampleVolumes  RUNTIME DESTINATION bin)

ADD_EXECUTABLE(ResampleVolumeToIsotropic ResampleVolumeToIsotropic.cxx )
TARGET_LINK_LIBRARIES(ResampleVolumeToIsotropic ${ITK_LIBRARIES} tinyxml)
INSTALL(TARGETS ResampleVolumeToIsotropic  RUNTIME DESTINATION bin)

#ADD_EXECUTABLE(TranslationTransform TranslationTransform.cxx )
#TARGET_LINK_LIBRARIES(TranslationTransform ${ITK_LIBRARIES} tinyxml)
#INSTALL(TARGETS TranslationTransform  RUNTIME DESTINATION bin)
