FILE(GLOB Optimize_sources *.cpp)
FILE(GLOB Optimize_headers *.h *.txx)

set(SW_PYTHON_LIBS pybind11::embed)

if (APPLE)
  include_directories(${_Python3_INCLUDE_DIR})
  set(SW_PYTHON_LIBS "")
endif(APPLE)

add_library(Optimize STATIC
  ${Optimize_sources}
  )

message("IIIIIHHDSJHFKJS Found Torch under ${TORCH_LIBRARIES}")

target_link_libraries(Optimize PUBLIC
  ${VTK_LIBRARIES}
  OpenVDB::openvdb
  Alignment
  trimesh2
  Mesh
  tinyxml
  Utils
  Particles
  ${ITK_LIBRARIES}
  igl::core
  ${Boost_LIBRARIES}
  ${TORCH_LIBRARIES}
  ${GEOMETRYCENTRAL_LIBRARY}
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
  $<BUILD_INTERFACE:${PYTHON_INCLUDE_DIRS}>
  $<INSTALL_INTERFACE:include>
  )

# Install
set_target_properties(Optimize PROPERTIES PUBLIC_HEADER "${Optimize_headers}")

install(TARGETS Optimize EXPORT ShapeWorksTargets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION include/Optimize
  )
