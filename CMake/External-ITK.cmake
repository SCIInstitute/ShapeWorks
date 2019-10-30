#---------------------------------------------------------------------------
# Get and build ITK

set(SHAPEWORKS_ITK_VERSION v5.0.1 CACHE STRING "ITK version")
mark_as_advanced(SHAPEWORKS_ITK_VERSION)
set(SHAPEWORKS_ITK_VERSION_STRING 5.0 CACHE INTERNAL "ITK version string")
set(ITK_DIR ${CMAKE_INSTALL_PREFIX}/lib/cmake/ITK-${SHAPEWORKS_ITK_VERSION_STRING})

ExternalProject_Add(ITK
  GIT_REPOSITORY https://github.com/InsightSoftwareConsortium/ITK.git
  GIT_TAG "${SHAPEWORKS_ITK_VERSION}"
  PREFIX external
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS
    ${ep_common_args}
    -DBUILD_SHARED_LIBS:BOOL=ON
    -DBUILD_EXAMPLES:BOOL=OFF
    -DBUILD_TESTING:BOOL=OFF
    -DVTK_DIR:PATH=${VTK_DIR}
    -DModule_ITKVtkGlue:BOOL=ON
    -DITK_USE_SYSTEM_VXL=on
    -DVXL_DIR=${VXL_DIR}
  UPDATE_COMMAND ""
  LOG_DOWNLOAD 0
  LOG_UPDATE 0
  DEPENDS vtk vxl
)
