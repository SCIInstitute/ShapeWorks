FILE(GLOB ParticleSystem_sources ./ParticleSystem/*.t* ./ParticleSystem/*.cxx ./ParticleSystem/*.cpp ./ParticleSystem/*.h)

add_library(Optimize STATIC
  ${ParticleSystem_sources}
  Optimize.cpp
  OptimizeParameterFile.cpp
  OptimizeParameters.cpp
  ./ParticleSystem/MeshDomain.cpp
  ./ParticleSystem/Constraints.cpp
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
  ${ITK_LIBRARIES}
  ${VTK_LIBRARIES}
  igl::core
  )

# Create an interface library that will bring pybind11::embed along
# This can't be used to create the shared libary for the Python API
# Otherwise, you have two pythons and the init clashes an gives a
# GIL mutex error
add_library(OptimizeLibraries INTERFACE)

target_link_libraries(OptimizeLibraries INTERFACE
  Optimize
  pybind11::embed
  )

target_include_directories(Optimize PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ParticleSystem>
  $<BUILD_INTERFACE:${PYTHON_INCLUDE_DIRS}>
  $<INSTALL_INTERFACE:include>
  )
