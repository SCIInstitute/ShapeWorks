PROJECT(shapeworks)
cmake_minimum_required(VERSION 2.6)
#SET(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_STANDARD 11)

# use ccache if available
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

#set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
#include(DeployQt5)


if(UNIX AND NOT APPLE)
  set(LINUX TRUE)
endif()

if(LINUX)
  # We need RPATH rather than RUNPATH since RUNPATH
  # though recommended, is not used for transitive dependencies
  SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--disable-new-dtags")
endif(LINUX)


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







if(APPLE)
    set(CMAKE_INSTALL_RPATH "@executable_path/../lib")
else()
    set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
endif()
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)


add_subdirectory(ExternalLibs)
add_subdirectory(Libs)


option(Build_PrepTools "Build prep tools" ON)
option(Build_Run "Build shapeworks" ON)
option(Build_Post "Build post" ON)
option(Build_View2 "Build view2" OFF)
option(Build_Studio "Build studio" OFF)

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

include_directories ( ${CMAKE_CURRENT_SOURCE_DIR} )



if(Build_PrepTools)
    include_directories ( ${SHAPEWORKS_SOURCE_DIR}/Prep/Source)
    add_subdirectory(Prep/Source)
endif(Build_PrepTools)
if(Build_Run)
    include_directories ( ${SHAPEWORKS_SOURCE_DIR}/Run/source)
    add_subdirectory(Run/source)
endif(Build_Run)
if(Build_Post)
    include_directories ( ${SHAPEWORKS_SOURCE_DIR}/Post/source/ShapeWorksPost-V1)
    add_subdirectory(Post/source/ShapeWorksPost-V1)
endif(Build_Post)
if(Build_View2)
    include_directories ( ${SHAPEWORKS_SOURCE_DIR}/Post/source/ShapeWorksView2)
    add_subdirectory(Post/source/ShapeWorksView2)
endif(Build_View2)
if(Build_Studio)
  add_subdirectory(Studio/src)
endif(Build_Studio)


add_custom_command(
    TARGET ShapeWorksView2 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:Qt5::Widgets>
        $<TARGET_FILE:Qt5::Core>
        $<TARGET_FILE:Qt5::Gui>
        $<TARGET_FILE:Qt5::Sql>
        $<TARGET_FILE_DIR:ShapeWorksView2>
)


#INSTALL(FILES $<TARGET_FILE:Qt5::Sql> DESTINATION lib)
#INSTALL(FILES $<TARGET_FILE:Qt5::Core> DESTINATION lib)
#INSTALL(FILES $<TARGET_FILE:Qt5::Gui> DESTINATION lib)
#INSTALL(FILES $<TARGET_FILE:Qt5::Widgets> DESTINATION lib)

set(APP ${CMAKE_INSTALL_PREFIX}/stl2vtk) 

# set paths to dependencies
set(DIRS ${CMAKE_INSTALL_PREFIX}/lib )


foreach(plugin ${Qt5Network_PLUGINS})
  get_target_property(_loc ${plugin} LOCATION)
  message("Plugin ${plugin} is at location ${_loc}")
endforeach()



get_filename_component(Qt_BIN_DIR "${QT_QMAKE_EXECUTABLE}" PATH)
get_filename_component(Qt_PREFIX_DIR "${Qt_BIN_DIR}" PATH)

  
get_filename_component(QT_LOC Qt5::Widgets DIRECTORY)
MESSAGE(STATUS "QT libs: ${QT_LOC}")

set(qtlib "Widgets")
#GET_FILENAME_COMPONENT(QT_LIB_DIR_tmp ${QT_${qtlib}_LIBRARY_RELEASE} PATH)

MESSAGE(STATUS "QT libs: ${QT_LIB_DIR_tmp}")

get_filename_component(qt_dlls_dir "${QtWidgets_location}" PATH)
get_filename_component(qt_dlls_dir "${qt_dlls_dir}" PATH)



get_target_property(QT5_WIDGETSLIBRARY Qt5::Widgets LOCATION)
get_filename_component(QT5_WIDGETSLIBRARYPATH ${QT5_WIDGETSLIBRARY} PATH)
MESSAGE(STATUS "QT widget: ${QT5_WIDGETSLIBRARYPATH}")
set(DIRS "${QT5_WIDGETSLIBRARYPATH} \${CMAKE_INSTALL_PREFIX}/lib")

set(DIRS "${CMAKE_INSTALL_PREFIX}/lib")

list(APPEND DIRS "${QT5_WIDGETSLIBRARYPATH}") 


include(InstallRequiredSystemLibraries)

MESSAGE(STATUS "CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}")

#install_qt5_executable(${APP} "" "" "${DIRS}" "" "")

# copy all libraries to the binary folder
    # by calling fixup_bundle, when TARGET INSTALL is executed
    # install(CODE "
    #    include(BundleUtilities)
    #    fixup_bundle(\"${APP}\"   \"\"   \"${DIRS}\")
    #    " 
    #    COMPONENT libraries
    #    )


