SET(ANALYZE_MOC_HDRS
  MeshManager.h
  QMeshWarper.h
  MeshWorker.h
)

qt5_wrap_cpp( ANALYZE_MOC_SRCS ${ANALYZE_MOC_HDRS} )

SET(Analyze_headers
  Analyze.h
  CustomSurfaceReconstructionFilter.h
  LegacyMeshGenerator.h
  MeshCache.h
  MeshGenerator.h
  MeshGroup.h
  MeshManager.h
  MeshWorkQueue.h
  MeshWorker.h
  Particles.h
  QMeshWarper.h
  Shape.h
  StudioEnums.h
  StudioMesh.h
  SurfaceReconstructor.h
  Vis.h
  vtkPolyDataToImageData.h
 )

add_library(Analyze STATIC
  Analyze.cpp
  CustomSurfaceReconstructionFilter.cpp
  LegacyMeshGenerator.cpp
  MeshCache.cpp
  MeshGenerator.cpp
  MeshGroup.cpp
  MeshManager.cpp
  MeshWorkQueue.cpp
  MeshWorker.cpp
  Particles.cpp
  QMeshWarper.cpp
  Shape.cpp
  StudioEnums.cpp
  StudioMesh.cpp
  SurfaceReconstructor.cpp
  Vis.cpp
  vtkPolyDataToImageData.cpp
  ${ANALYZE_MOC_SRCS}
  )

target_include_directories(Analyze PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>)

target_link_libraries(Analyze PUBLIC
  Mesh
  Particles
  Utils
  )

# set
set_target_properties(Analyze PROPERTIES PUBLIC_HEADER
  "${Analyze_headers}")

install(TARGETS Analyze EXPORT ShapeWorksTargets
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION lib
  LIBRARY DESTINATION lib
  PUBLIC_HEADER DESTINATION include/Analyze
  )
