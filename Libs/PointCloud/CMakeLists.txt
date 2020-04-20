# Build
find_package(Eigen3 3.3 REQUIRED NO_MODULE)

add_subdirectory(evaluators)
add_subdirectory(solvers)

FILE(GLOB KERNELS_SOURCE "kernels/*")

set(PointCloud_sources
  PointCloud.cpp
  ${KERNELS_SOURCE}
  )
set(PointCloud_headers
  PointCloud.h
  )

add_library(PointCloud STATIC
  ${PointCloud_sources}
  ${PointCloud_headers})
target_include_directories(PointCloud PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>)

add_dependencies(PointCloudEvaluators PointCloudSolvers)
target_link_libraries (PointCloud PUBLIC
    PointCloudEvaluators
    PointCloudSolvers
    Eigen3::Eigen)

# Install
set_target_properties(PointCloud PROPERTIES PUBLIC_HEADER
  "${PointCloud_headers}")
install(TARGETS PointCloud
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION lib
  LIBRARY DESTINATION lib
  PUBLIC_HEADER DESTINATION include
  )

ADD_EXECUTABLE(RBFShapeBuild RBFShapeBuild.cxx)
TARGET_LINK_LIBRARIES(RBFShapeBuild PointCloud Eigen3::Eigen ${VTK_LIBRARIES}  ${ITK_LIBRARIES} tinyxml)
INSTALL(TARGETS RBFShapeBuild RUNTIME DESTINATION bin)

ADD_EXECUTABLE(RBFShapeEvaluate RBFShapeEvaluate.cxx)
TARGET_LINK_LIBRARIES(RBFShapeEvaluate PointCloud Eigen3::Eigen ${VTK_LIBRARIES}  ${ITK_LIBRARIES} tinyxml)
INSTALL(TARGETS RBFShapeEvaluate RUNTIME DESTINATION bin)

ADD_EXECUTABLE(RBFShapeGradient RBFShapeGradient.cxx)
TARGET_LINK_LIBRARIES(RBFShapeGradient PointCloud Eigen3::Eigen ${VTK_LIBRARIES}  ${ITK_LIBRARIES} tinyxml)
INSTALL(TARGETS RBFShapeGradient RUNTIME DESTINATION bin)
