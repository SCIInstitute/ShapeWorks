FILE(GLOB SRCS "*.txx" "*.h" "*.cxx")
add_library(ITKParticleSystem STATIC
  ${SRCS}
  )
target_link_libraries(ITKParticleSystem Alignment ParticleSystemUtils fim_v4)
target_include_directories(ITKParticleSystem PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<INSTALL_INTERFACE:include>)
