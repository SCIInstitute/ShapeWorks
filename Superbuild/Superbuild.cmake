# maybe need to build using `cmake --build . -j 16` in order to pass parallel to dependent projects

include(ExternalProject)

set(ep_common_args
  -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
  -DCMAKE_INSTALL_PREFIX:STRING=${CMAKE_INSTALL_PREFIX}
  -DCMAKE_CXX_COMPILER:STRING=${CMAKE_CXX_COMPILER}
  -DCMAKE_C_COMPILER:STRING=${CMAKE_C_COMPILER}
  -DCMAKE_GENERATOR:STRING=${CMAKE_GENERATOR}
  -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${CMAKE_BINARY_DIR}/bin
  )

# Compute -G arg for configuring external projects with the same CMake generator:
if(CMAKE_EXTRA_GENERATOR)
  set(gen "${CMAKE_EXTRA_GENERATOR} - ${CMAKE_GENERATOR}")
else()
  set(gen "${CMAKE_GENERATOR}" )
endif()

set(ShapeWorks_DEPENDENCIES )

if(NOT VTK_DIR)
  include(${CMAKE_CURRENT_LIST_DIR}/External-VTK.cmake)
  list(APPEND ShapeWorks_DEPENDENCIES vtk)
endif()

if(NOT VXL_DIR)
  include(${CMAKE_CURRENT_LIST_DIR}/External-VXL.cmake)
  list(APPEND ShapeWorks_DEPENDENCIES vxl)
endif()

if(NOT ITK_DIR)
  include(${CMAKE_CURRENT_LIST_DIR}/External-ITK.cmake)
  list(APPEND ShapeWorks_DEPENDENCIES ITK)
endif()

#<ctc> the ShapeWorks cmakelists supposedly supports openmp on osx
set(SHAPEWORKS_USE_OPENMP ON)
if(APPLE)
  set(SHAPEWORKS_USE_OPENMP OFF)
endif()

ExternalProject_Add(ShapeWorks
  DEPENDS ${ShapeWorks_DEPENDENCIES}
  DOWNLOAD_COMMAND ""
  UPDATE_COMMAND ""
#<ctc> note desire for top-level CMakeLists to be placed in CMake directory, so this would stay the same, but just to note.
  SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/..
  PREFIX shapeworks
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS
    ${ep_common_args}
    # ${_build_doc_args}
    # ${_python_args}
    -DBUILD_SHARED_LIBS:BOOL=FALSE
    #ShapeWorks (remainder of arguments)
#<ctc> TODO: the various options probably need to be in this file
    -DBuild_PrepTools=ON
    -DBuild_Run=ON
    -DBuild_Post=OFF
    -DBuild_View2=OFF
    -DBuild_Studio=OFF
    -DUSE_OPENMP=${SHAPEWORKS_USE_OPENMP} 
    -DSHAPEWORKS_PYTHON_API=OFF
    -DVTK_DIR:PATH=${VTK_DIR}
    -DVXL_DIR:PATH=${VXL_DIR}
    -DITK_DIR:PATH=${ITK_DIR}
    -Wno-dev
    -Wno-deprecated
)

