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

ADD_EXECUTABLE(ResampleVolumeToIsotropic ResampleVolumeToIsotropic.cxx )
TARGET_LINK_LIBRARIES(ResampleVolumeToIsotropic ${ITK_LIBRARIES} tinyxml)
INSTALL(TARGETS ResampleVolumeToIsotropic  RUNTIME DESTINATION bin)

ADD_EXECUTABLE(ICPRigid3DMeshRegistration ICPRigid3DMeshRegistration.cxx)
TARGET_LINK_LIBRARIES(ICPRigid3DMeshRegistration ${VTK_LIBRARIES} tinyxml)
INSTALL(TARGETS ICPRigid3DMeshRegistration  RUNTIME DESTINATION bin)
