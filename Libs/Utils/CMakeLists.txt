# Build
set(Utils_sources
  Utils.cpp
  StringUtils.cpp
  )

set(Utils_headers
  Utils.h
  StringUtils.h
  )

add_library(Utils STATIC
  ${Utils_sources}
  ${Utils_headers})

target_include_directories(Utils PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>
  )

target_link_libraries(Utils PUBLIC
  Eigen3::Eigen 
  ${VTK_LIBRARIES}
  )

# Install
set_target_properties(Utils PROPERTIES PUBLIC_HEADER
  "${Utils_headers}")
install(TARGETS Utils
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION lib
  LIBRARY DESTINATION lib
  PUBLIC_HEADER DESTINATION include
  )

ADD_EXECUTABLE(VTKUnstructuredGridToPolyData VTKUnstructuredGridToPolyData.cxx)
TARGET_LINK_LIBRARIES(VTKUnstructuredGridToPolyData ${VTK_LIBRARIES} tinyxml)
INSTALL(TARGETS VTKUnstructuredGridToPolyData  RUNTIME DESTINATION bin)

ADD_EXECUTABLE(FeaFromVTK FeaFromVTK.cxx)
TARGET_LINK_LIBRARIES(FeaFromVTK ${VTK_LIBRARIES} tinyxml)
INSTALL(TARGETS FeaFromVTK  RUNTIME DESTINATION bin)
