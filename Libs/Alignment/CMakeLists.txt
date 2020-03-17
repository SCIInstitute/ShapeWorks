set(Transforms_sources
  Transforms/itkAdvancedTransform.cpp
  Transforms/itkSparseKernelTransform.cpp
  Transforms/itkCompactlySupportedRBFSparseKernelTransform.cpp
  Transforms/itkThinPlateSplineKernelTransform2.cpp
  Transforms/itkKernelTransform2.cpp)
set(Procrustes_sources
  Procrustes3D.cpp)
add_library(Alignment STATIC
  ${Procrustes_sources}
  ${Transforms_sources})
target_include_directories(Alignment PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Transforms>
  $<INSTALL_INTERFACE:include>)

ADD_EXECUTABLE(ResizeOriginResampleVolumes ResizeOriginResampleVolumes.cxx )
TARGET_LINK_LIBRARIES(ResizeOriginResampleVolumes ${ITK_LIBRARIES} tinyxml)
INSTALL(TARGETS ResizeOriginResampleVolumes  RUNTIME DESTINATION bin)

ADD_EXECUTABLE(ResampleVolumeToIsotropic ResampleVolumeToIsotropic.cxx )
TARGET_LINK_LIBRARIES(ResampleVolumeToIsotropic ${ITK_LIBRARIES} tinyxml)
INSTALL(TARGETS ResampleVolumeToIsotropic  RUNTIME DESTINATION bin)

ADD_EXECUTABLE(ICPRigid3DImageRegistration ICPRigid3DImageRegistration.cxx)
TARGET_LINK_LIBRARIES(ICPRigid3DImageRegistration ${ITK_LIBRARIES} Utils)
INSTALL(TARGETS ICPRigid3DImageRegistration  RUNTIME DESTINATION bin)

ADD_EXECUTABLE(ApplyRigid3DTransformationToImage ApplyRigid3DTransformationToImage.cxx)
TARGET_LINK_LIBRARIES(ApplyRigid3DTransformationToImage ${ITK_LIBRARIES} Utils)
INSTALL(TARGETS ApplyRigid3DTransformationToImage  RUNTIME DESTINATION bin)

ADD_EXECUTABLE(ICPRigid3DMeshRegistration ICPRigid3DMeshRegistration.cxx)
TARGET_LINK_LIBRARIES(ICPRigid3DMeshRegistration ${VTK_LIBRARIES} tinyxml)
INSTALL(TARGETS ICPRigid3DMeshRegistration  RUNTIME DESTINATION bin)

ADD_EXECUTABLE(ReflectMeshes ReflectMeshes.cxx )
TARGET_LINK_LIBRARIES(ReflectMeshes ${VTK_LIBRARIES} tinyxml)
INSTALL(TARGETS ReflectMeshes  RUNTIME DESTINATION bin)

add_executable(TranslateImages TranslateImages.cxx)
target_link_libraries(TranslateImages ${ITK_LIBRARIES} Utils)
INSTALL(TARGETS TranslateImages  RUNTIME DESTINATION bin)

