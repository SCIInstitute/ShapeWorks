PROJECT( ShapeWorksStudio )

cmake_minimum_required(VERSION 2.8)  

###########################################
# Flags for Microsoft Compiler
###########################################

IF (WIN32 AND MSVC)
  ADD_DEFINITIONS(-D_WIN32_WINNT=0x0501 -DNTDDI_VERSION=0x05010000)
#  ADD_DEFINITIONS(-D_WIN32_WINNT=0x0501)
  ADD_DEFINITIONS(-DPSAPI_VERSION=1)
  # Disable Visual C++ Secure Warnings
  ADD_DEFINITIONS(-D_SCL_SECURE_NO_WARNINGS)
  ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
  ADD_DEFINITIONS(-D_BIND_TO_CURRENT_VCLIBS_VERSION=1)
  ADD_DEFINITIONS(-D_BIND_TO_CURRENT_CRT_VERSION=1)
  SET(CMAKE_CXX_FLAGS "/Oi ${CMAKE_CXX_FLAGS} /MP4")
ENDIF (WIN32 AND MSVC)

set(CMAKE_BUILD_TYPE Release)

#--------------------------------------------------------------------------------
# ITK, VTK, Qt5
#--------------------------------------------------------------------------------
FIND_PACKAGE(VTK COMPONENTS
  vtkCommonCore
  vtkInfovisCore
  vtkInteractionStyle
  vtkViewsQt
  vtkFiltersCore
  vtkGUISupportQt
  vtkRenderingQt
)

INCLUDE( ${VTK_USE_FILE} )
MESSAGE(STATUS "** VTK_DIR: ${VTK_DIR}")

FIND_PACKAGE(ITK REQUIRED)
IF (ITK_FOUND)
  INCLUDE (${ITK_USE_FILE})
ENDIF(ITK_FOUND)
MESSAGE(STATUS "** ITK_DIR: ${ITK_DIR}")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set (CMAKE_AUTOMOC ON)
# Find QT5

find_package(OpenGL REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)

cmake_policy(SET CMP0020 NEW)

#headers
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

FILE(GLOB STUDIO_UI  */*.ui)
QT5_WRAP_UI(UI_SRC ${STUDIO_UI})

#--------------------------------------------------------------------------------
### Application
#--------------------------------------------------------------------------------

FILE(GLOB STUDIO_SRCS *.cc */*.cc */*.cpp)
FILE(GLOB STUDIO_HDRS */*.h)

SET(STUDIO_RCS Resources/ShapeWorksStudio.qrc)

# we need this to be able to include headers produced by uic in our code
# (CMAKE_BINARY_DIR holds a path to the build directory)
INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )
#INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )

# For Apple set the icns file containing icons
IF(APPLE)
  # set how it shows up in the Info.plist file
  SET(MACOSX_BUNDLE_ICON_FILE ShapeWorksStudio.icns) 
  # set where in the bundle to put the icns file
  SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/Resources/ShapeWorksStudio.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  # include the icns file in the target
  SET(STUDIO_SRCS ${STUDIO_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/Resources/ShapeWorksStudio.icns)
ENDIF(APPLE)

#IF(WIN32)
#  SET(STUDIO_SRCS ${STUDIO_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/Resources/ShapeWorksStudio.rc)
#ENDIF(WIN32)

# here we instruct CMake to build STUDIO executable from all of the source files
ADD_EXECUTABLE( ShapeWorksStudio MACOSX_BUNDLE WIN32 
  ${STUDIO_SRCS}
  ${STUDIO_HDRS}
  ${STUDIO_UI} 
  )

TARGET_LINK_LIBRARIES( ShapeWorksStudio
  ${ITK_LIBRARIES}
  ${Boost_THREAD_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_FILESYSTEM_LIBRARY}
  ${VTK_LIBRARIES} 
  ${OPENGL_LIBRARIES}
  Qt5::Widgets
  Qt5::OpenGL
  Qt5::Core
  Qt5::Gui
  Qt5::WinMain
  )

#--------------------------------------------------------------------------------
### Remove Below Section Eventually
#--------------------------------------------------------------------------------
  
# we need to have the shapeworks command line tools for studio to work. TODO --- REPLACE WITH NEW LIBRARY!
find_file(ShapeworksCLT_BINARY_DIR
  NAMES
  shapeworks
  shapeworks/bin
  shapeworks/build/bin
  shapeworks/build/bin/Release
  PATHS
  $ENV{HOME}
  "C:/"
  $ENV{HOME}/Documents
  )
SET(required_deps
  ShapeWorksGroom
  ShapeWorksRun
  )
foreach (file ${required_deps})
  find_file(${file}-dependency
    NAMES
    ${file}
    ${file}.exe
    PATHS
    ${ShapeworksCLT_BINARY_DIR}
    NO_DEFAULT_PATH
    )
  if (${${file}-dependency} MATCHES "${file}-dependency-NOTFOUND")
    set(ShapeworksCLT_BINARY_DIR "ShapeworksCLT_BINARY_DIR-NOTFOUND" CACHE PATH "Shapeworks command line tools binary directory" FORCE)
  endif()
  unset(${file}-dependency CACHE)
endforeach()

if (${ShapeworksCLT_BINARY_DIR} MATCHES "ShapeworksCLT_BINARY_DIR-NOTFOUND")
  message(ERROR " Shapeworks command line tools binary directory not found. Set ShapeworksCLT_BINARY_DIR in CMake.")
else()
  #copy the binaries from ShapeworksCLT to Cleaver binary dir
  message(STATUS "ShapeworksCLT found at: ${ShapeworksCLT_BINARY_DIR}")
  FILE(GLOB SHAPEWORKS_DEPS
    ${ShapeworksCLT_BINARY_DIR}/ShapeWorksGroom*
    ${ShapeworksCLT_BINARY_DIR}/ShapeWorksRun*
    )
  foreach(file ${SHAPEWORKS_DEPS})
		add_custom_command(TARGET ShapeWorksStudio POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy ${file} 
				   $<TARGET_FILE_DIR:ShapeWorksStudio>)
  endforeach()
  # platform specific filenames
  if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    add_definitions(-DGROOM_EXECUTABLE="ShapeWorksGroom.exe")
    add_definitions(-DOPTIMIZE_EXECUTABLE="ShapeWorksRun.exe")
  else()
    add_definitions(-DGROOM_EXECUTABLE="ShapeWorksGroom")
    add_definitions(-DOPTIMIZE_EXECUTABLE="ShapeWorksRun")
  endif()
endif()

#--------------------------------------------------------------------------------
# INSTALLATION AND PACKAGING
#--------------------------------------------------------------------------------
SET(plugin_dest_dir bin)
SET(qtconf_dest_dir bin)
SET(APPS "\${CMAKE_INSTALL_PREFIX}/bin/ShapeWorksStudio")
IF(APPLE)
  SET(plugin_dest_dir ShapeWorksStudio.app/Contents/MacOS)
  SET(qtconf_dest_dir ShapeWorksStudio.app/Contents/Resources)
  SET(contents_dest_dir ShapeWorksStudio.app/Contents)
  SET(APPS "\${CMAKE_INSTALL_PREFIX}/ShapeWorksStudio.app")
ENDIF(APPLE)
IF(WIN32)
  SET(APPS "\${CMAKE_INSTALL_PREFIX}/bin/ShapeWorksStudio.exe")
ENDIF(WIN32)

# Install the STUDIO application, on Apple, the bundle is at the root of the
# install tree, and on other platforms it'll go into the bin directory.
INSTALL(TARGETS ShapeWorksStudio
    BUNDLE DESTINATION . COMPONENT Runtime
    RUNTIME DESTINATION bin COMPONENT Runtime
    )

# Install needed Qt plugins by copying directories from the qt installation
# One can cull what gets copied by using 'REGEX "..." EXCLUDE'
INSTALL(DIRECTORY "${QT_PLUGINS_DIR}/imageformats" DESTINATION ${plugin_dest_dir}/plugins COMPONENT Runtime)

# Install needed Qt .nib files and others
IF(APPLE)
INSTALL(DIRECTORY "${QT_QTGUI_LIBRARY_RELEASE}/Versions/Current/Resources" DESTINATION ${contents_dest_dir} COMPONENT Runtime)
ENDIF(APPLE)

# install a qt.conf file
# this inserts some cmake code into the install script to write the file
INSTALL(CODE "
    file(WRITE \"\${CMAKE_INSTALL_PREFIX}/${qtconf_dest_dir}/qt.conf\" \"\")
    " COMPONENT Runtime)
	
# Use BundleUtilities to get all other dependencies for the application to work.
# It takes a bundle or executable along with possible plugins and inspects it
# for dependencies.  If they are not system dependencies, they are copied.

# directories to look for dependencies
SET(DIRS ${QT_LIBRARY_DIRS})

# Now the work of copying dependencies into the bundle/package
# The quotes are escaped and variables to use at install time have their $ escaped
# An alternative is the do a configure_file() on a script and use install(SCRIPT  ...).
# Note that the image plugins depend on QtSvg and QtXml, and it got those copied
# over.
INSTALL(CODE "
    file(GLOB_RECURSE QTPLUGINS
      \"\${CMAKE_INSTALL_PREFIX}/${plugin_dest_dir}/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
    include(BundleUtilities)
    fixup_bundle(\"${APPS}\" \"\${QTPLUGINS}\" \"${DIRS}\")
    " COMPONENT Runtime)
	
# To Create a package, one can run "cpack -G DragNDrop CPackConfig.cmake" on Mac OS X
# where CPackConfig.cmake is created by including CPack
# And then there's ways to customize this as well
set(CPACK_BINARY_DRAGNDROP ON)
include(CPack)

INSTALL(FILES README.txt DESTINATION .)

#--------------------------------------------------------------------------------
#-- Add an Option to toggle the generation of the API documentation
#--------------------------------------------------------------------------------
option(BUILD_DOCUMENTATION "Use Doxygen to create the HTML based API documentation" OFF)
if(BUILD_DOCUMENTATION)
  FIND_PACKAGE(Doxygen)
  if (NOT DOXYGEN_FOUND)
    message(FATAL_ERROR 
      "Doxygen is needed to build the documentation. Please install it correctly")
  endif()
  #-- Configure the Template Doxyfile for our specific project
  configure_file(Doxyfile.in 
                 ${PROJECT_BINARY_DIR}/Doxyfile  @ONLY IMMEDIATE)
  #-- Add a custom target to run Doxygen when ever the project is built
  add_custom_target (docs
    COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile
    SOURCES ${PROJECT_BINARY_DIR}/Doxyfile)
  # IF you do NOT want the documentation to be generated EVERY time you build the project
  # then leave out the 'ALL' keyword from the above command.
endif()
