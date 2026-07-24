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

# NOTE: CorrespondenceEvaluation.cpp uses Project, but we deliberately do NOT
# link Particles against Project here. Project pulls in xlnt, which bundles its
# own copy of expat and exports the public XML_* symbols. Adding a Project ->
# xlnt edge to this low-level library hoists xlnt ahead of VTK's/ITK's static
# expat archives in every consumer's link line, corrupting expat's encoding
# tables and segfaulting on the first .vtp read (broke OptimizeTests and
# shapeworks_py's .vtp reads). Because CorrespondenceEvaluation is only
# referenced by the shapeworks CLI and the Python module -- both of which
# already link Project -- its object is only pulled from libParticles.a by
# those targets, and its Project symbols resolve there. Targets that link
# Particles but never reference CorrespondenceEvaluation (OptimizeTests, etc.)
# never pull the object and keep their original, safe link order.

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
