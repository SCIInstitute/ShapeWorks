#---------------------------------------------------------------------------
# Get and build vxl

set(SHAPEWORKS_VXL_VERSION v2.0.2 CACHE STRING "vxl version")
set(VXL_DIR ${CMAKE_INSTALL_PREFIX}/share/vxl/cmake)

ExternalProject_Add(vxl
  GIT_REPOSITORY https://github.com/vxl/vxl.git
  GIT_TAG "${SHAPEWORKS_VXL_VERSION}"
  PREFIX external
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS
    ${ep_common_args}
    -DBUILD_CORE_VIDEO:BOOL=OFF
    -DBUILD_BRL:BOOL=OFF
    -DBUILD_CONTRIB:BOOL=OFF
    -DVNL_CONFIG_LEGACY_METHODS=ON
    -DVCL_STATIC_CONST_INIT_FLOAT=0
    -DVXL_FORCE_V3P_GEOTIFF:BOOL=ON
    -DVXL_USE_GEOTIFF:BOOL=OFF
    -DVXL_USE_DCMTK:BOOL=OFF
    -DBUILD_SHARED_LIBS:BOOL=ON
    -DBUILD_EXAMPLES:BOOL=OFF
    -DBUILD_TESTING:BOOL=OFF
    -DCMAKE_BUILD_TYPE=Release
    "-DCMAKE_CXX_FLAGS=-Wno-dev -Wno-deprecated"
  UPDATE_COMMAND ""
  LOG_DOWNLOAD 0
  LOG_UPDATE 0
)

