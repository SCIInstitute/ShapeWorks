

FILE(GLOB SRCS "*.txx" "*.h" "*.cxx")

ADD_LIBRARY (ITKParticleSystem STATIC
${SRCS}
)

if(BUILD_MeshSupport OR BUILD_FeaMeshSupport)
  target_link_libraries(ITKParticleSystem fim_v4)
endif(BUILD_MeshSupport OR BUILD_FeaMeshSupport)
target_link_libraries(ITKParticleSystem Utilities)


# if(FIM_FOUND AND BUILD_MeshSupport)
  # target_link_libraries(ITKParticleSystem ${FIM_LIB})
# endif(FIM_FOUND AND BUILD_MeshSupport)
