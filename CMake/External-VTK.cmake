#---------------------------------------------------------------------------
# Get and build vtk

set(SHAPEWORKS_VTK_VERSION v8.2.0 CACHE STRING "vtk version")
mark_as_advanced(SHAPEWORKS_VTK_VERSION)
set(SHAPEWORKS_VTK_VERSION_STRING 8.2 CACHE INTERNAL "vtk version string")
set(VTK_DIR ${CMAKE_INSTALL_PREFIX}/lib/cmake/vtk-${SHAPEWORKS_VTK_VERSION_STRING})

ExternalProject_Add(vtk
  GIT_REPOSITORY https://gitlab.kitware.com/vtk/vtk.git
  GIT_TAG "${SHAPEWORKS_VTK_VERSION}"
  PREFIX external
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS
    ${ep_common_args}
    -DBUILD_SHARED_LIBS:BOOL=ON
    -DBUILD_TESTING:BOOL=OFF
    -DVTK_Group_Web:BOOL=OFF
    -DVTK_Group_Qt:BOOL=${SHAPEWORKS_BUILD_GUI}
    -DVTK_QT_VERSION=5
    -DVTK_PYTHON_VERSION=3
    -DCMAKE_BUILD_TYPE=Release
    -DCMAKE_INSTALL_LIBDIR=lib 
  UPDATE_COMMAND ""
  LOG_DOWNLOAD 0
  LOG_UPDATE 0
)
