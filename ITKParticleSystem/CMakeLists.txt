

FILE(GLOB SRCS "*.txx" "*.h" "*.cxx")

ADD_LIBRARY (ITKParticleSystem STATIC
${SRCS}
)

if(BUILD_MeshSupport)
  target_link_libraries(ITKParticleSystem fim)
endif(BUILD_MeshSupport)

# if(FIM_FOUND AND BUILD_MeshSupport)
  # target_link_libraries(ITKParticleSystem ${FIM_LIB})
# endif(FIM_FOUND AND BUILD_MeshSupport)
