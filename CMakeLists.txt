cmake_minimum_required(VERSION 3.11)
set(CMAKE_CXX_STANDARD 11)             # available options are [98, 11, 14, 17. 20]
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake")

#
# ShapeWorks options
#
option(USE_OPENMP      "Build using OpenMP" ON)
option(Build_Post      "Build post"         OFF)
option(Build_View2     "Build view2"        OFF)
option(Build_Studio    "Build studio"       OFF)
if (${Build_Post} OR ${Build_View2} OR ${Build_Studio})
  set(SHAPEWORKS_GUI ON)
  set(SHAPEWORKS_QT_REQUIRED REQUIRED)
else()
  set(SHAPEWORKS_GUI OFF)
endif()
message(STATUS "SHAPEWORKS_GUI: ${SHAPEWORKS_GUI}")

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/install" CACHE STRING "Install path prefix, prepended onto install directories.")

set(SHAPEWORKS_EXTERNALS_CMAKE_BUILD_TYPE Release CACHE STRING "Build type for external libraries (default Release)")
mark_as_advanced(SHAPEWORKS_EXTERNALS_CMAKE_BUILD_TYPE)

#
# Start with ShapeWorksExternals, which adds external libs.
# Then add ShapeWorks itself as an external project, causing it to 
# configure the rest (below) by setting SHAPEWORKS_SUPERBUILD to OFF.
#
set(SHAPEWORKS_SUPERBUILD ON CACHE BOOL "")
mark_as_advanced(SHAPEWORKS_SUPERBUILD)
if (SHAPEWORKS_SUPERBUILD)
  include(ShapeworksExternals)

  if (APPLE)
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
    INSTALL_COMMAND ""
    SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}"
    PREFIX shapeworks
    BUILD_ALWAYS TRUE
    CMAKE_GENERATOR ${gen}
    CMAKE_ARGS
      ${ep_common_args}
      -DBUILD_SHARED_LIBS:BOOL=FALSE

      # ShapeWorks-specific arguments
      -DBuild_Run=${Build_Run}
      -DBuild_Post=${Build_Post}
      -DBuild_View2=${Build_View2}
      -DBuild_Studio=${Build_Studio}
      -DUSE_OPENMP=${USE_OPENMP} 
      -DSHAPEWORKS_PYTHON_API=OFF
      -DVTK_DIR:PATH="${VTK_DIR}"
      -DVXL_DIR:PATH="${VXL_DIR}"
      -DITK_DIR:PATH="${ITK_DIR}"
      -DSHAPEWORKS_SUPERBUILD:BOOL=OFF
      "-DCMAKE_CXX_FLAGS=${SHAPEWORKS_CXX_FLAGS_STR}"
    )
else()
  #
  # Next, actually configure ShapeWorks using the parameters passed from
  # ExternalProject_Add above. Keep this in mind when modifying configs.
  #
  project(ShapeWorks)
  include(DefaultBuildType)

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

  # GUI
  if(APPLE)
    cmake_policy(SET CMP0072 NEW) # prefer the new opengl library GLVND to the legacy if both are present
  endif()
  find_package(Qt5 COMPONENTS Core Widgets OpenGL Gui Sql ${SHAPEWORKS_QT_REQUIRED})
  if (Qt5Core_FOUND AND Qt5Widgets_FOUND AND Qt5OpenGL_FOUND AND Qt5Gui_FOUND AND Qt5Sql_FOUND)
    set(SHAPEWORKS_QT_FOUND 1)
  else()
    set(SHAPEWORKS_QT_FOUND 0)
  endif()

  if (SHAPEWORKS_GUI)
    # headers (TODO: move to shapeworks gui targets' includes)
    include_directories(${Qt5Widgets_INCLUDE_DIRS})
    include_directories(${Qt5OpenGL_INCLUDE_DIRS})
    include_directories(${Qt5Core_INCLUDE_DIRS})
    include_directories(${Qt5Gui_INCLUDE_DIRS})
    include_directories(${Qt5Sql_INCLUDE_DIRS})

    # defines (TODO: move to shapeworks gui targets' includes)
    add_definitions(${Qt5Widgets_DEFINITIONS})
    add_definitions(${Qt5OpenGL_DEFINITIONS})
    add_definitions(${Qt5Core_DEFINITIONS})
    add_definitions(${Qt5Gui_DEFINITIONS})
    add_definitions(${Qt5Sql_DEFINITIONS})

    if (WIN32)
      cmake_policy(SET CMP0020 NEW)   # link to qtmain.lib on Windows
    else()
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-write-strings")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
    endif()
  endif()

  ### vtk
  find_package(VTK ${SHAPEWORKS_VTK_VERSION_MIN})
  if (NOT VTK_FOUND)
    message(FATAL_ERROR "** VTK not found. By default cmake will download it (see BUILD.md)")
  endif()
  include(${VTK_USE_FILE})

  ### vxl
  FIND_PACKAGE(VXL REQUIRED)
  IF(VXL_FOUND)
    # this adds deprecated VXL variables to the cmake namespace (TODO: use the new ones)
    INCLUDE (${VXL_CMAKE_DIR}/UseVXL.cmake)
  ELSE(VXL_FOUND)
    MESSAGE(SEND_ERROR "VXL is required, but could not be found.")
  ENDIF(VXL_FOUND)

  ### ITK
  FIND_PACKAGE(ITK ${SHAPEWORKS_ITK_VERSION_MIN} REQUIRED)
  IF (ITK_FOUND)
    INCLUDE(${ITK_USE_FILE})
  ELSE()
    MESSAGE(SEND_ERROR "ITK (The Insight Toolkit) is required, but could not be found.")
  ENDIF()

  if(USE_OPENMP)
    find_package(OPENMP REQUIRED)
  endif(USE_OPENMP)

  if(APPLE)
    set(CMAKE_INSTALL_RPATH "@executable_path/../lib")
  else()
    set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
  endif()

  add_subdirectory(ExternalLibs)    # TODO: cmake externalproject_add should be used for libs in this directory
  add_subdirectory(Libs)
  #add_subdirectory(Applications)   # coming soon...

  include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

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
  set(DIRS "${CMAKE_INSTALL_PREFIX}/lib")
  list(APPEND DIRS "${QT5_WIDGETSLIBRARYPATH}") 

  MESSAGE(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
  MESSAGE(STATUS "CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}")

endif()
