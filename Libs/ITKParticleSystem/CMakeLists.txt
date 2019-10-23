

FILE(GLOB SRCS "*.txx" "*.h" "*.cxx")

ADD_LIBRARY (ITKParticleSystem STATIC
${SRCS}
)
target_link_libraries(ITKParticleSystem Procrustes ParticleSystemUtils)
target_include_directories(ITKParticleSystem PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<INSTALL_INTERFACE:include>)

#if(BUILD_MeshSupport OR BUILD_FeaMeshSupport)
target_link_libraries(ITKParticleSystem fim_v4)
#endif(BUILD_MeshSupport OR BUILD_FeaMeshSupport)

# if(FIM_FOUND AND BUILD_MeshSupport)
  # target_link_libraries(ITKParticleSystem ${FIM_LIB})
# endif(FIM_FOUND AND BUILD_MeshSupport)
