FILE(GLOB ParticleSystem_sources ./ParticleSystem/*.t* ./ParticleSystem/*.cxx)
add_library(Optimize STATIC
  ${ParticleSystem_sources})
target_link_libraries(Optimize
  Alignment
  trimesh2
  Mesh
  tinyxml)
target_include_directories(Optimize PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ParticleSystem>
  $<INSTALL_INTERFACE:include>)

add_executable(ShapeWorksRun ShapeWorksRun.cxx ShapeWorksRunApp.h ShapeWorksRunApp.txx)
set_target_properties(ShapeWorksRun PROPERTIES COMPILE_DEFINITIONS SW_USE_FEAMESH)
target_link_libraries(ShapeWorksRun ${ITK_LIBRARIES} ${VTK_LIBRARIES} tinyxml Mesh vgl vgl_algo Optimize Utils trimesh2)
install(TARGETS ShapeWorksRun RUNTIME DESTINATION bin)

if(BUILD_TESTS)
  add_subdirectory(Test)
endif(BUILD_TESTS)
