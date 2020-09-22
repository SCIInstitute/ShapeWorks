find_package(pybind11 2.4.3 REQUIRED)

FILE(GLOB ParticleSystem_sources ./ParticleSystem/*.t* ./ParticleSystem/*.cxx ./ParticleSystem/*.cpp ./ParticleSystem/*.h)

add_library(Optimize STATIC
  ${ParticleSystem_sources}
  Optimize.cpp
  OptimizeParameterFile.cpp
  ./ParticleSystem/MeshDomain.cpp
  ./ParticleSystem/TriMeshWrapper.cpp
  ./ParticleSystem/OptimizationVisualizer.cpp
  )

target_link_libraries(Optimize
  OpenVDB::openvdb
  Alignment
  trimesh2
  Mesh
  tinyxml
  Utils
  Particles
  pybind11::embed
  ${ITK_LIBRARIES}
  ${VTK_LIBRARIES}
  vgl
  vgl_algo
  )

target_include_directories(Optimize PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ParticleSystem>
  $<INSTALL_INTERFACE:include>
  )

add_executable(ShapeWorksRun ShapeWorksRun.cxx )
set_target_properties(ShapeWorksRun PROPERTIES COMPILE_DEFINITIONS SW_USE_FEAMESH)
target_link_libraries(ShapeWorksRun Optimize)
install(TARGETS ShapeWorksRun RUNTIME DESTINATION bin)

