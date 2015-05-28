ADD_LIBRARY (ITKParticleSystem STATIC
Procrustes3D.cxx
itkParticleSystem.cxx
itkParticleProcrustesRegistration.cxx
)

if(FIM_FOUND AND BUILD_MeshSupport)
  target_link_libraries(ITKParticleSystem ${FIM_LIB})
endif(FIM_FOUND AND BUILD_MeshSupport)
