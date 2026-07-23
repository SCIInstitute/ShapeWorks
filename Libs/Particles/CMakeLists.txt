# Particles library

set(Particles_sources
  ParticleSystemEvaluation.cpp
  ParticleShapeStatistics.cpp
  ShapeEvaluation.cpp
  CorrespondenceEvaluation.cpp
  ReconstructSurface.cpp
  ParticleNormalEvaluation.cpp
  ParticleFile.cpp
  )

set(Particles_headers
  ParticleSystemEvaluation.h
  ParticleShapeStatistics.h
  EvaluationUtil.h
  ShapeEvaluation.h
  CorrespondenceEvaluation.h
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

# Project is only used inside CorrespondenceEvaluation.cpp — keep PRIVATE so it
# does not propagate to consumers of Particles (which was found to perturb VTK
# static-init/link ordering in shapeworks_py and break .vtp reads).
target_link_libraries(Particles PRIVATE
  Project
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
