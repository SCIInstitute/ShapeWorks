# Particles library

set(Particles_sources
 ParticleSystem.cpp
 ParticleShapeStatistics.cpp
 ShapeEvaluation.cpp
 ReconstructSurface.cpp
 ParticleNormalEvaluation.cpp
 InvertibleNetworkStats.cpp
 )

set(Particles_headers
 ParticleSystem.h
 ParticleShapeStatistics.h
 itkParticlePositionReader.h
 itkParticlePositionWriter.h
 EvaluationUtil.h
 ShapeEvaluation.h
 ReconstructSurface.h
 ParticleNormalEvaluation.h
 InvertibleNetworkStats.h
 )

add_library(Particles STATIC
  ${Particles_sources}
  ${Particles_headers}
  )

target_include_directories(Particles PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>
  )

target_link_libraries(Particles PUBLIC
  Mesh
  Optimize
  tinyxml
  Eigen3::Eigen
  ${TORCH_LIBRARIES}
  )

# set
set_target_properties(Particles PROPERTIES PUBLIC_HEADER
  "${Particles_headers}"
  )

install(TARGETS Particles EXPORT ShapeWorksTargets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION include
  )
