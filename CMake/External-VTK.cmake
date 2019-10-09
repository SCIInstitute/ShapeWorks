#---------------------------------------------------------------------------
# Get and build vtk

set(SHAPEWORKS_VTK_VERSION v8.2.0 CACHE STRING "vtk version")
set(SHAPEWORKS_VTK_VERSION_STRING 8.2 CACHE INTERNAL "vtk version string")
#set(PYTHON_EXECUTABLE "${ITKPYTHON_EXECUTABLE}")
set(VTK_DIR ${CMAKE_INSTALL_PREFIX}/lib/cmake/vtk-${SHAPEWORKS_VTK_VERSION_STRING})

ExternalProject_Add(vtk
  GIT_REPOSITORY https://gitlab.kitware.com/vtk/vtk.git
  GIT_TAG "${SHAPEWORKS_VTK_VERSION}"
  PREFIX external
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS
    ${ep_common_args}
    #${_wrap_python_args}
    -DBUILD_SHARED_LIBS:BOOL=ON
    -DBUILD_TESTING:BOOL=OFF
    -DVTK_Group_Web:BOOL=OFF
    #should be: -DVTK_Group_Qt:BOOL=${SHAPEWORKS_BUILD_GUI}
    -DVTK_Group_Qt:BOOL=ON   #${SHAPEWORKS_BUILD_GUI}
    # -DVTK_Group_Imaging:BOOL=ON
    # -DVTK_Group_Views:BOOL=ON
    # -DVTK_WRAP_PYTHON:BOOL=OFF
    # -DPYTHON_EXECUTABLE:FILEPATH=${PYTHON_EXECUTABLE}
    # -DExternalData_OBJECT_STORES:STRING=${ExternalData_OBJECT_STORES}
    -DVTK_QT_VERSION=5
    -DVTK_PYTHON_VERSION=3
    -DCMAKE_BUILD_TYPE=Release
    -DCMAKE_INSTALL_LIBDIR=lib 
    "-DCMAKE_CXX_FLAGS=-Wno-dev"
  UPDATE_COMMAND ""
  LOG_DOWNLOAD 0
  LOG_UPDATE 0
)
