# Particles library

set(Particles_sources
  ParticleSystemEvaluation.cpp
  ParticleShapeStatistics.cpp
  ShapeEvaluation.cpp
  ReconstructSurface.cpp
  ParticleNormalEvaluation.cpp
  ParticleFile.cpp
  )

set(Particles_headers
  ParticleSystemEvaluation.h
  ParticleShapeStatistics.h
  itkParticlePositionReader.h
  itkParticlePositionWriter.h
  EvaluationUtil.h
  ShapeEvaluation.h
  ReconstructSurface.h
  ParticleNormalEvaluation.h
  ParticleFile.h
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
