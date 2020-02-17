set(sources
        ParticleSystem.cpp
        itkParticleShapeStatistics.cpp
        itkParticlePositionReader.cpp
        itkParticlePositionWriter.cpp
        ShapeEvaluation.cpp)
set(headers
        ParticleSystem.h
        itkParticleShapeStatistics.h
        itkParticlePositionReader.h
        itkParticlePositionWriter.h
        EvaluationUtil.h
        ShapeEvaluation.h)

add_library(Particles STATIC
  ${sources}
  ${headers})
target_include_directories(Particles PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>)
target_link_libraries(Particles PUBLIC
  tinyxml
  Eigen3::Eigen
  Mesh
  Alignment
  Utils
  Optimize)

# set
set_target_properties(Particles PROPERTIES PUBLIC_HEADER
  "${Particles_headers}")
install(TARGETS Analyze
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION lib
  LIBRARY DESTINATION lib
  PUBLIC_HEADER DESTINATION include
  )
