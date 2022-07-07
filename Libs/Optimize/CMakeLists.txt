FILE(GLOB ParticleSystem_sources ./ParticleSystem/*.t* ./ParticleSystem/*.cxx ./ParticleSystem/*.cpp ./ParticleSystem/*.h)

set(SW_PYTHON_LIBS pybind11::embed)

if (APPLE)
  include_directories(${_Python3_INCLUDE_DIR})
  set(SW_PYTHON_LIBS "")
endif(APPLE)

add_library(Optimize STATIC
  ${ParticleSystem_sources}
  Optimize.cpp
  OptimizeParameterFile.cpp
  OptimizeParameters.cpp
  ParticleSystem/MeshDomain.cpp
  ParticleSystem/Constraints.cpp
  ParticleSystem/Constraint.cpp
  ParticleSystem/SphereConstraint.cpp
  ParticleSystem/PlaneConstraint.cpp
  ParticleSystem/FreeFormConstraint.cpp
  ParticleSystem/TriMeshWrapper.cpp
  ParticleSystem/VtkMeshWrapper.cpp
  ParticleSystem/OptimizationVisualizer.cpp
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
  ${Boost_LIBRARIES}
  geometry-central
  nlohmann_json::nlohmann_json
  ${SW_PYTHON_LIBS}
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
