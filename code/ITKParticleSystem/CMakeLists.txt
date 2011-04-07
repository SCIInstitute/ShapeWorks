ADD_LIBRARY (ITKParticleSystem STATIC
Procrustes3D.cxx
itkParticleSystem.cxx
itkParticleProcrustesRegistration.cxx
)

IF(NOT APPLE)
TARGET_LINK_LIBRARIES(ITKParticleSystem fim_lib trimesh freeglut_static)
ELSE(NOT APPLE)
TARGET_LINK_LIBRARIES(ITKParticleSystem fim_lib trimesh)
ENDIF(NOT APPLE)
