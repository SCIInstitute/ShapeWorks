# ShapeWorks Superbuild
#  Includes external libraries automatically which are built at compilation time rather than during configuration.

project(ShapeWorksSuperbuild)
include(ExternalProject)

# CMake arguments common to all the external projects
set(ep_common_args
  -DCMAKE_BUILD_TYPE:STRING=${SHAPEWORKS_EXTERNALS_CMAKE_BUILD_TYPE}
  -DCMAKE_INSTALL_PREFIX:STRING=${CMAKE_INSTALL_PREFIX}
  -DCMAKE_CXX_COMPILER:STRING=${CMAKE_CXX_COMPILER}
  -DCMAKE_C_COMPILER:STRING=${CMAKE_C_COMPILER}
  -DCMAKE_GENERATOR:STRING=${CMAKE_GENERATOR}
  -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${CMAKE_BINARY_DIR}/bin
  -DCMAKE_CXX_FLAGS=""
  )

# Compute -G arg for configuring external projects with the same CMake generator:
if(CMAKE_EXTRA_GENERATOR)
  set(gen "${CMAKE_EXTRA_GENERATOR} - ${CMAKE_GENERATOR}")
else()
  set(gen "${CMAKE_GENERATOR}" )
endif()

set(ShapeWorks_DEPENDENCIES )

if(NOT VTK_DIR)
  include("${CMAKE_CURRENT_LIST_DIR}/External-VTK.cmake")
  list(APPEND ShapeWorks_DEPENDENCIES vtk)
endif()

if(NOT VXL_DIR)
  include("${CMAKE_CURRENT_LIST_DIR}/External-VXL.cmake")
  list(APPEND ShapeWorks_DEPENDENCIES vxl)
endif()

if(NOT ITK_DIR)
  include("${CMAKE_CURRENT_LIST_DIR}/External-ITK.cmake")
  list(APPEND ShapeWorks_DEPENDENCIES ITK)
endif()

#FIXME: ShapeWorks cmakelists seems to support openmp on osx, but it's not compiling
set(SHAPEWORKS_USE_OPENMP ON)
if(APPLE)
  set(SHAPEWORKS_USE_OPENMP OFF)
endif()

if (APPLE)
  # doesn't work in latest clang: -Wno-dev 
  set(SHAPEWORKS_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wno-deprecated -Wno-inconsistent-missing-override)
else()
  # TODO: make sure these are straight for each [version of each] platform
  set(SHAPEWORKS_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wno-deprecated -Wno-inconsistent-missing-override)
endif()
  
# cxx flags must be passed as on the command line, with spaces between args
string(REPLACE ";" " " SHAPEWORKS_CXX_FLAGS_STR "${SHAPEWORKS_CXX_FLAGS}")

# ShapeWorks (this will include ShapeWorks.cmake)
ExternalProject_Add(ShapeWorks
  DEPENDS ${ShapeWorks_DEPENDENCIES}
  DOWNLOAD_COMMAND ""
  UPDATE_COMMAND ""
  SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.."
  PREFIX shapeworks
  BUILD_ALWAYS TRUE
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS
    ${ep_common_args}
    -DBUILD_SHARED_LIBS:BOOL=FALSE

    # ShapeWorks-specific arguments
    -DBuild_PrepTools=ON
    -DBuild_Run=ON
    -DBuild_Post=ON
    -DBuild_View2=ON
    -DBuild_Studio=ON
    -DUSE_OPENMP=${SHAPEWORKS_USE_OPENMP} 
    -DSHAPEWORKS_PYTHON_API=OFF
    -DVTK_DIR:PATH="${VTK_DIR}"
    -DVXL_DIR:PATH="${VXL_DIR}"
    -DITK_DIR:PATH="${ITK_DIR}"
    -DSHAPEWORKS_SUPERBUILD:BOOL=OFF
    "-DCMAKE_CXX_FLAGS=${SHAPEWORKS_CXX_FLAGS_STR}"
)

