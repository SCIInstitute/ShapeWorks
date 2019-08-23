PROJECT(shapeworks)
cmake_minimum_required(VERSION 2.6)
SET(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_STANDARD 11)

# use ccache if available
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()


SET(BUILD_EXECS_AT_SAME_LEVEL TRUE CACHE BOOL "For use with ShapeWorksRemote, build all executable outputs in the same directory." )
IF (BUILD_EXECS_AT_SAME_LEVEL)
  SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/binary)
  SET(CMAKE_BUNDLE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/binary)
ENDIF (BUILD_EXECS_AT_SAME_LEVEL)

FIND_PACKAGE(ITK REQUIRED)
IF (ITK_USE_FILE)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_USE_FILE)
  MESSAGE(SEND_ERROR "ITK (The Insight Toolkit) is required, but could not be found.")
ENDIF(ITK_USE_FILE)

FIND_PACKAGE(VXL)
IF (VXL_FOUND)
  INCLUDE (${VXL_CMAKE_DIR}/UseVXL.cmake)
ELSE(VXL_FOUND)
  MESSAGE(SEND_ERROR "VXL is required, but could not be found.")
ENDIF (VXL_FOUND)


# Optionally use OpenMP
option(USE_OPENMP "Build parallel optimization using OpenMP" ON)

# Find OpenMP
if(APPLE AND USE_OPENMP)
  if(CMAKE_C_COMPILER_ID MATCHES "Clang")
    set(OpenMP_C "${CMAKE_C_COMPILER}")
    set(OpenMP_C_FLAGS "-fopenmp=libomp -Wno-unused-command-line-argument")
    set(OpenMP_C_LIB_NAMES "libomp" "libgomp" "libiomp5")
    set(OpenMP_libomp_LIBRARY ${OpenMP_C_LIB_NAMES})
    set(OpenMP_libgomp_LIBRARY ${OpenMP_C_LIB_NAMES})
    set(OpenMP_libiomp5_LIBRARY ${OpenMP_C_LIB_NAMES})
  endif()
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(OpenMP_CXX "${CMAKE_CXX_COMPILER}")
    set(OpenMP_CXX_FLAGS "-fopenmp=libomp -Wno-unused-command-line-argument")
    set(OpenMP_CXX_LIB_NAMES "libomp" "libgomp" "libiomp5")
    set(OpenMP_libomp_LIBRARY ${OpenMP_CXX_LIB_NAMES})
    set(OpenMP_libgomp_LIBRARY ${OpenMP_CXX_LIB_NAMES})
    set(OpenMP_libiomp5_LIBRARY ${OpenMP_CXX_LIB_NAMES})
  endif()
endif()

if(USE_OPENMP)
  find_package(OpenMP REQUIRED)
  add_definitions(-DSW_USE_OPENMP)
  add_definitions(-DMP_USE_OPENMP)
  if(APPLE)
    # the find_package flags just don't work for the latest clang, and I'm also not sure how to find the include path.
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Xpreprocessor -fopenmp -lomp -Wno-unused-command-line-argument -I\"/Users/cam/tools/miniconda3/envs/shapeworks_build-000/include\" -L\"/Users/cam/tools/miniconda3/envs/shapeworks_build-000/lib\"")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xpreprocessor -fopenmp -lomp -Wno-unused-command-line-argument -I\"/Users/cam/tools/miniconda3/envs/shapeworks_build-000/include\" -L\"/Users/cam/tools/miniconda3/envs/shapeworks_build-000/lib\"")
  else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")  #these don't exist, see cmake FindOpenMP (https://github.com/Kitware/CMake/blob/master/Modules/FindOpenMP.cmake)
  endif(APPLE)
endif(USE_OPENMP)

# if (OPENMP_FOUND)
#   include_directories("${OPENMP_INCLUDES}")
#   message("OpenMP_C_FOUND: ${OpenMP_C_FOUND}")
#   message("OpenMP_C_FLAGS: ${OpenMP_C_FLAGS}")
#   message("OpenMP_C_LIB_NAMES/LIBRARY/LIBRARIES: ${OpenMP_C_LIB_NAMES} ${OpenMP_C_LIBRARY} ${OpenMP_C_LIBRARIES}")
#   message("OpenMP_C_FLAGS: ${OpenMP_C_FLAGS}")
#   message("OpenMP_CXX_LIBRARIES: ${OPENMP_CXX_LIBRARIES}")
#   message("OpenMP_C_LIBRARIES: ${OPENMP_C_LIBRARIES}")
#   message("OpenMP_CXX_LIBRARIES: ${OPENMP_CXX_LIBRARIES}")
#   link_directories("${OPENMP_C_LIBRARIES}")
#   link_directories("${OPENMP_CXX_LIBRARIES}")
#   set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#   set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#   # set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
# endif(OPENMP_FOUND)

# if(USE_OPENMP)
#   FIND_PACKAGE( OpenMP REQUIRED)
#   # find_package(OpenMP) not working on APPLE [w/ conda-forge openmp] (https://github.com/fospald/fibergen/issues/5#issuecomment-440497345)
#   if(APPLE)
#     add_definitions(-DSW_USE_OPENMP)
#     add_definitions(-DMP_USE_OPENMP)
#     if(CMAKE_C_COMPILER_ID MATCHES "Clang")
#       set(OpenMP_C "${CMAKE_C_COMPILER}")
#       set(OpenMP_C_FLAGS "-fopenmp=libomp -Wno-unused-command-line-argument")
#       set(OpenMP_C_LIB_NAMES "libomp" "libgomp" "libiomp5")
#       set(OpenMP_libomp_LIBRARY ${OpenMP_C_LIB_NAMES})
#       set(OpenMP_libgomp_LIBRARY ${OpenMP_C_LIB_NAMES})
#       set(OpenMP_libiomp5_LIBRARY ${OpenMP_C_LIB_NAMES})
#     else()
#       MESSAGE(SEND_ERROR "OpenMP: CMAKE_C_COMPILER is not Clang.")
#     endif()
#     if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
#       set(OpenMP_CXX "${CMAKE_CXX_COMPILER}")
#       set(OpenMP_CXX_FLAGS "-fopenmp=libomp -Wno-unused-command-line-argument")
#       set(OpenMP_CXX_LIB_NAMES "libomp" "libgomp" "libiomp5")
#       set(OpenMP_libomp_LIBRARY ${OpenMP_CXX_LIB_NAMES})
#       set(OpenMP_libgomp_LIBRARY ${OpenMP_CXX_LIB_NAMES})
#       set(OpenMP_libiomp5_LIBRARY ${OpenMP_CXX_LIB_NAMES})
#     else()
#       MESSAGE(SEND_ERROR "OpenMP: CMAKE_CXX_COMPILER is not Clang.")
#     endif()
#   else()
#     FIND_PACKAGE( OpenMP REQUIRED)
#     if(OPENMP_FOUND)
#       message("Found OpenMP")
#       add_definitions(-DSW_USE_OPENMP)
#       add_definitions(-DMP_USE_OPENMP)
#       set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#       set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#       set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
#     else()
#       MESSAGE(SEND_ERROR "OpenMP not found.")
#     endif()
#   endif()
# endif(USE_OPENMP)


INCLUDE_DIRECTORIES(
  Libs/fim_v4
  Libs/fim_v4/trimesh2/include
  Libs/tinyxml
  Libs/OptionParser
  Libs/PreViewMeshQC
  Libs/ITKVTK
  Libs/Procrustes
  Libs/Eigen
  Libs/Transforms
  Libs/SurfaceReconstruction
  )

#todo: subdirs is deprecated: use add_subdirectory
SUBDIRS(Libs/fim_v4)
SUBDIRS(Libs/tinyxml)
SUBDIRS(Libs/OptionParser)
SUBDIRS(Libs/PreViewMeshQC)
SUBDIRS(Libs/ITKVTK)
SUBDIRS(Libs/Procrustes)
SUBDIRS(Libs/Transforms)
SUBDIRS(Libs/SurfaceReconstruction)


option(Build_PrepTools "Build prep tools" ON)
option(Build_Run "Build shapeworks" ON)
option(Build_Post "Build post" ON)
option(Build_View2 "Build view2" OFF)  #todo: figure out how to install old qt 4.8.1 on new osx (other platforms may also have issues)

#todo: setup INSTALL paths as described in Trello (otherwise 'make install' just puts everything in the root path of the specified installation directory)

# if(Build_View2)
#   set(VTK_QT_OPTIONS "vtkViewsQt vtkGUISupportQt vtkRenderingQt")
# endif(Build_View2)
  
# FIND_PACKAGE(VTK COMPONENTS
#   vtkCommonCore
#   vtkIOCore
#   vtkIOLegacy
#   vtkIOPLY
#   vtkIOXML
#   vtkIOImage
#   vtkIOGeometry
#   vtkInfovisCore
#   vtkInteractionStyle
#   vtkFiltersCore
#   vtkFiltersModeling
#   vtkRenderingAnnotation
#   vtkInteractionWidgets
#   vtkImagingStencil
#   ${VTK_QT_OPTIONS}
#   REQUIRED)
# IF (VTK_FOUND)
#   INCLUDE (${VTK_USE_FILE})
# ELSE(VTK_FOUND)
#   MESSAGE(FATAL_ERROR "VTK could not be found.")
# ENDIF (VTK_FOUND)


find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
MESSAGE(STATUS "** USE_VTK_FILE: ${VTK_USE_FILE}")

if(Build_PrepTools)
    include_directories ( ${SHAPEWORKS_SOURCE_DIR}/Prep/Source)
    subdirs(Prep/Source)
endif(Build_PrepTools)
if(Build_Run)
    include_directories ( ${SHAPEWORKS_SOURCE_DIR}/Run/source)
    subdirs(Run/source)
endif(Build_Run)
if(Build_Post)
    include_directories ( ${SHAPEWORKS_SOURCE_DIR}/Post/source/ShapeWorksPost-V1)
    subdirs(Post/source/ShapeWorksPost-V1)
endif(Build_Post)
if(Build_View2)
    include_directories ( ${SHAPEWORKS_SOURCE_DIR}/Post/source/ShapeWorksView2)
    subdirs(Post/source/ShapeWorksView2)
endif(Build_View2)


