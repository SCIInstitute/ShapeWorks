# still can't find gtest/gtest.h, not sure why
# needed to test private functions
#find_package(GTest REQUIRED)

# Mesh library

set(Mesh_sources
 Mesh.cpp
 meshFIM.cpp
 MeshUtils.cpp
 MeshWarper.cpp
 MeshComputeThickness.cpp
 )

set(Mesh_headers
 Mesh.h
 meshFIM.h
 MeshUtils.h
 MeshWarper.h
 )

FILE(GLOB PreviewMeshQC_sources ./PreviewMeshQC/*.cpp)
FILE(GLOB PreviewMeshQC_headers ./PreviewMeshQC/*.h)

add_library(Mesh STATIC
 ${Mesh_sources}
 ${Mesh_headers}
 ${PreviewMeshQC_sources}
 ${PreviewMeshQC_headers}
 )

target_include_directories(Mesh PUBLIC
 $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
 $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PreviewMeshQC>
 $<INSTALL_INTERFACE:include>
 )

target_link_libraries(Mesh PUBLIC
  ${VTK_LIBRARIES}
  Image
  Common
  Alignment
  Utils
  trimesh2
  Eigen3::Eigen
  igl::core
  Particles
  TBB::tbb
  ${GEOMETRYCENTRAL_LIBRARY}
  Boost::filesystem
  acvd::acvd
  )

# Install
set_target_properties(Mesh PROPERTIES PUBLIC_HEADER
 "${Mesh_headers};${PreviewMeshQC_headers}"
 )

install(TARGETS Mesh EXPORT ShapeWorksTargets
 ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
 RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}
 LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
 PUBLIC_HEADER DESTINATION include/Mesh
 )




#ADD_EXECUTABLE(ComputeGeodesicDistanceToCurve ComputeGeodesicDistanceToCurve.cxx)
#TARGET_LINK_LIBRARIES(ComputeGeodesicDistanceToCurve ${ITK_LIBRARIES} Mesh tinyxml)
#INSTALL(TARGETS ComputeGeodesicDistanceToCurve RUNTIME DESTINATION bin)

# Added to backlog
#ADD_EXECUTABLE(GenerateFidsFiles GenerateFidsFiles.cxx)
#TARGET_LINK_LIBRARIES(GenerateFidsFiles ${ITK_LIBRARIES} Mesh tinyxml)
#INSTALL(TARGETS GenerateFidsFiles RUNTIME DESTINATION bin)

# Added to backlog
#ADD_EXECUTABLE(GenerateFidsFilesFromMeshes GenerateFidsFilesFromMeshes.cxx)
#TARGET_LINK_LIBRARIES(GenerateFidsFilesFromMeshes ${ITK_LIBRARIES} Mesh tinyxml)
#INSTALL(TARGETS GenerateFidsFilesFromMeshes RUNTIME DESTINATION bin)

# Added to backlog
# ADD_EXECUTABLE(RemoveFidsDTLeakage RemoveFidsDTLeakage.cxx)
# TARGET_LINK_LIBRARIES(RemoveFidsDTLeakage ${ITK_LIBRARIES} Mesh tinyxml)
# INSTALL(TARGETS RemoveFidsDTLeakage RUNTIME DESTINATION bin)
