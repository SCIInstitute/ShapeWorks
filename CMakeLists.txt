cmake_minimum_required(VERSION 3.10.2)
set(CMAKE_CXX_STANDARD 11)             # available options are [98, 11, 14, 17. 20]
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake")

#
# ShapeWorks options
#
option(USE_OPENMP      "Build using OpenMP" ON)
option(Build_PrepTools "Build prep tools"   ON)
option(Build_Run       "Build shapeworks"   ON)
option(Build_Post      "Build post"         ON)
option(Build_View2     "Build view2"        OFF)
option(Build_Studio    "Build studio"       OFF)
if (${Build_Post} OR ${Build_View2} OR ${Build_Studio})
  set(SHAPEWORKS_GUI ON)
else()
  set(SHAPEWORKS_GUI OFF)
endif()
message(STATUS "SHAPEWORKS_GUI: ${SHAPEWORKS_GUI}")  #removeme, just to verify

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/install" CACHE STRING "Install path prefix, prepended onto install directories.")
set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the build type, options are: None Debug Release RelWithDebInfo MinSizeRel")
set(SHAPEWORKS_EXTERNALS_CMAKE_BUILD_TYPE Release CACHE STRING "Build type for external libraries")
mark_as_advanced(SHAPEWORKS_EXTERNALS_CMAKE_BUILD_TYPE)

#
# Start with Superbuild, which adds external libs then builds ShapeWorks itself by setting this to OFF.
#
set(SHAPEWORKS_SUPERBUILD ON CACHE bool "")
mark_as_advanced(SHAPEWORKS_SUPERBUILD)
if (SHAPEWORKS_SUPERBUILD)
  include(Superbuild)
else()
  # actually build ShapeWorks
  project(ShapeWorks)

  # use ccache if available
  find_program(CCACHE_PROGRAM ccache)
  if(CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
  endif()

  if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
  endif()

  if(LINUX)
    # We need RPATH rather than RUNPATH since RUNPATH
    # though recommended, is not used for transitive dependencies
    SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--disable-new-dtags")
  endif(LINUX)

  # put executables in bin directory (under build dir) so they are easy to find
  SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
  SET(CMAKE_BUNDLE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

  if (SHAPEWORKS_GUI)
    set(SHAPEWORKS_VTK_GUI_COMPONENTS vtkGUISupportQt)

    # Find QT5
    find_package(OpenGL REQUIRED)
    find_package(Qt5Widgets REQUIRED)
    find_package(Qt5OpenGL REQUIRED)
    find_package(Qt5Core REQUIRED)
    find_package(Qt5Gui REQUIRED)

    cmake_policy(SET CMP0020 NEW)   # be sure to link to qtmain.lib on Windows

    #headers  #<ctc> move to shapeworks_gui interface target includes
    include_directories(${Qt5Widgets_INCLUDE_DIRS})
    include_directories(${Qt5OpenGL_INCLUDE_DIRS})
    include_directories(${Qt5Core_INCLUDE_DIRS})
    include_directories(${Qt5Gui_INCLUDE_DIRS})

    #defines
    add_definitions(${Qt5Widgets_DEFINITIONS})
    add_definitions(${Qt5OpenGL_DEFINITIONS})
    add_definitions(${Qt5Core_DEFINITIONS})
    add_definitions(${Qt5Gui_DEFINITIONS})

    get_target_property(QtWidgets_location Qt5::Widgets LOCATION)
    get_target_property(QtGui_location Qt5::Gui LOCATION)
    get_target_property(QtCore_location Qt5::Core LOCATION)
    get_target_property(QtOpenGL_location Qt5::OpenGL LOCATION)

    if (NOT WIN32)
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-write-strings")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
    endif()
  endif()

  ### vtk
  message(STATUS "VTK_DIR is ${VTK_DIR}") #just verifying, can remove this output
  find_package(VTK ${SHAPEWORKS_VTK_VERSION_MIN} COMPONENTS 
    #vtkCommonColor  #TODO: identify which vtk components we use (by trial and error? there's also https://github.com/Kitware/VTK/blob/master/Utilities/Maintenance/WhatModulesVTK.py)
    vtkCommonCore
    # vtkCommonDataModel
    # vtkFiltersGeneral
    # vtkFiltersGeometry
    # vtkFiltersPoints
    # vtkIOExodus
    # vtkInteractionStyle
    # vtkRenderingFreeType
    # vtkRenderingGL2PSOpenGL2
    ${SHAPEWORKS_VTK_GUI_COMPONENTS}
    #QUIET
    )
  if (NOT VTK_FOUND)
    message(FATAL_ERROR "** VTK not found. Need to use Superbuild.cmake (see BUILD.md)")
  endif()

  message(STATUS "VXL_DIR is ${VXL_DIR}") #just verifying, can remove this output
  FIND_PACKAGE(VXL REQUIRED)
  IF(VXL_FOUND)
    # this adds deprecated VXL variables to the cmake namespace (TODO: use the new ones)
    INCLUDE (${VXL_CMAKE_DIR}/UseVXL.cmake)
  ELSE(VXL_FOUND)
    MESSAGE(SEND_ERROR "VXL is required, but could not be found.")
  ENDIF(VXL_FOUND)

  FIND_PACKAGE(ITK ${SHAPEWORKS_ITK_VERSION_MIN} REQUIRED)
  IF (ITK_FOUND)
    INCLUDE(${ITK_USE_FILE})
  ELSE()
    MESSAGE(SEND_ERROR "ITK (The Insight Toolkit) is required, but could not be found.")
  ENDIF()

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
      # set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Xpreprocessor -fopenmp -lomp -Wno-unused-command-line-argument -I\"/Users/cam/tools/miniconda3/envs/shapeworks_build-000/include\" -L\"/Users/cam/tools/miniconda3/envs/shapeworks_build-000/lib\"")
      # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xpreprocessor -fopenmp -lomp -Wno-unused-command-line-argument -I\"/Users/cam/tools/miniconda3/envs/shapeworks_build-000/include\" -L\"/Users/cam/tools/miniconda3/envs/shapeworks_build-000/lib\"")
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Xpreprocessor -fopenmp -lomp -Wno-unused-command-line-argument")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xpreprocessor -fopenmp -lomp -Wno-unused-command-line-argument")
    else()
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
      #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")  #these don't exist, see cmake FindOpenMP (https://github.com/Kitware/CMake/blob/master/Modules/FindOpenMP.cmake)
    endif(APPLE)
  endif(USE_OPENMP)

  if(APPLE)
    set(CMAKE_INSTALL_RPATH "@executable_path/../lib")
  else()
    set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
  endif()

  add_subdirectory(ExternalLibs)    # TODO: cmake should add any libs from this directory (currently only tinyxml)
  add_subdirectory(Libs)

  include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

  if(Build_PrepTools)
    add_subdirectory(Prep/Source)
  endif(Build_PrepTools)
  if(Build_Run)
    add_subdirectory(Run/source)
  endif(Build_Run)
  if(Build_Post)
    add_subdirectory(Post/source/ShapeWorksPost-V1)
  endif(Build_Post)
  if(Build_View2)
    MESSAGE(STATUS "Building View2")
    add_subdirectory(Post/source/ShapeWorksView2)
  endif(Build_View2)
  if(Build_Studio)
    add_subdirectory(Studio/src)
  endif(Build_Studio)


  set(APP ${CMAKE_INSTALL_PREFIX}/stl2vtk) 

  # set paths to dependencies
  set(DIRS ${CMAKE_INSTALL_PREFIX}/lib)

  foreach(plugin ${Qt5Network_PLUGINS})
    get_target_property(_loc ${plugin} LOCATION)
    message("Plugin ${plugin} is at location ${_loc}")
  endforeach()

  get_filename_component(Qt_BIN_DIR "${QT_QMAKE_EXECUTABLE}" PATH)
  get_filename_component(Qt_PREFIX_DIR "${Qt_BIN_DIR}" PATH)
  get_filename_component(QT_LOC Qt5::Widgets DIRECTORY)
  MESSAGE(STATUS "QT libs: ${QT_LOC}")

  set(qtlib "Widgets")
  MESSAGE(STATUS "QT libs: ${QT_LIB_DIR_tmp}")

  get_filename_component(qt_dlls_dir "${QtWidgets_location}" PATH)
  get_filename_component(qt_dlls_dir "${qt_dlls_dir}" PATH)

  get_target_property(QT5_WIDGETSLIBRARY Qt5::Widgets LOCATION)
  get_filename_component(QT5_WIDGETSLIBRARYPATH ${QT5_WIDGETSLIBRARY} PATH)
  MESSAGE(STATUS "QT widget: ${QT5_WIDGETSLIBRARYPATH}")
  set(DIRS "${QT5_WIDGETSLIBRARYPATH} \${CMAKE_INSTALL_PREFIX}/lib")

  set(DIRS "${CMAKE_INSTALL_PREFIX}/lib")

  list(APPEND DIRS "${QT5_WIDGETSLIBRARYPATH}") 

  # <ctc> try without this to see if it's actually necessary
  #include(InstallRequiredSystemLibraries)

  MESSAGE(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
  MESSAGE(STATUS "CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}")

endif()
