include_directories(${CMAKE_CURRENT_SOURCE_DIR})

#Match everything when analyzing dependencies
INCLUDE_REGULAR_EXPRESSION("^.*$")

# Optionally build with mesh support (requires linking against trimesh library)
#option(BUILD_MeshSupport "Build with Mesh support (requires fim library with trimesh2)" OFF)
# TODO: Do we need MeshSupport or can we remove this commented stuff?

# Fea mesh support
option(BUILD_FeaMeshSupport "Build with Fea Mesh support (requires fim library with trimesh2)" ON)

# Configure Mesh support
#if(BUILD_MeshSupport OR BUILD_FeaMeshSupport)
if(BUILD_FeaMeshSupport)
#  if(BUILD_MeshSupport)
#    add_definitions(-DSW_USE_MESH)
#  endif(BUILD_MeshSupport)
  if(BUILD_FeaMeshSupport)
    add_definitions(-DSW_USE_FEAMESH)
  endif(BUILD_FeaMeshSupport)
endif(BUILD_FeaMeshSupport)
#endif(BUILD_MeshSupport OR BUILD_FeaMeshSupport)

# Command-line tools are built by default.
subdirs(ShapeWorksRun)

IF (WIN32 AND MSVC)
  ADD_DEFINITIONS(-D_BIND_TO_CURRENT_VCLIBS_VERSION=1)
ENDIF (WIN32 AND MSVC)

