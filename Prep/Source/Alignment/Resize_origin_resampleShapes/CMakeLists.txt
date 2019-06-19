project(Resize_origin_resampleShapes)
cmake_minimum_required(VERSION 2.8)

ADD_EXECUTABLE(ResizeOriginResampleVolumes ResizeOriginResampleVolumes.cxx ${source_files})
TARGET_LINK_LIBRARIES(ResizeOriginResampleVolumes ${ITK_LIBRARIES} tinyxml)
INSTALL(TARGETS ResizeOriginResampleVolumes  RUNTIME DESTINATION .)

ADD_EXECUTABLE(ResampleVolumeToIsotropic ResampleVolumeToIsotropic.cxx ${source_files})
TARGET_LINK_LIBRARIES(ResampleVolumeToIsotropic ${ITK_LIBRARIES} tinyxml)
INSTALL(TARGETS ResampleVolumeToIsotropic  RUNTIME DESTINATION .)

#ADD_EXECUTABLE(TranslationTransform TranslationTransform.cxx ${source_files})
#TARGET_LINK_LIBRARIES(TranslationTransform ${ITK_LIBRARIES} tinyxml)
#INSTALL(TARGETS TranslationTransform  RUNTIME DESTINATION .)
