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
FILE(GLOB UI_RESOURCES_RCC Resources/*.qrc)
QT5_ADD_RESOURCES(STUDIO_RSCS ${UI_RESOURCES_RCC})

#--------------------------------------------------------------------------------
### Application
#--------------------------------------------------------------------------------

FILE(GLOB STUDIO_SRCS main.cc */*.cc */*.cpp */*.hxx)
FILE(GLOB STUDIO_HDRS */*.h)
FILE(GLOB STUDIO_ICNS Resources/Images/*.ico)
FILE(GLOB WINDOW_ICNS Resources/Images/shapes-icon.png)

add_definitions(-DICON_FILE="${WINDOW_ICNS}")
add_definitions(-DRSCS_FILE="${UI_RESOURCES_RCC}")

# we need this to be able to include headers produced by uic in our code
# (CMAKE_BINARY_DIR holds a path to the build directory)
INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}/Application )
INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}/Application/Groom )
INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}/Application/Optimize )
include_directories(${CMAKE_SOURCE_DIR}/ParticleShapeworks/include)
INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}/Groom )
INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}/Optimize )
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} )

# For Apple set the icns file containing icons
IF(APPLE)
  # set how it shows up in the Info.plist file
  SET(MACOSX_BUNDLE_ICON_FILE ShapeWorksStudio.icns)
  # set where in the bundle to put the icns file
  SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/Resources/ShapeWorksStudio.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  # include the icns file in the target
  SET(STUDIO_SRCS ${STUDIO_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/Resources/ShapeWorksStudio.icns)


ELSEIF(WIN32)
  ADD_DEFINITIONS(-DICON_RC_FILE="${CMAKE_CURRENT_SOURCE_DIR}/Resources/Images/shapes-icon.ico")
  SET(STUDIO_SRCS ${STUDIO_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/Resources/ShapeworksStudio.rc)
ENDIF(APPLE)
IF (${CMAKE_BUILD_TYPE} MATCHES "Debug")
  set(WinConsole "")
ELSE()
  set(WinConsole "WIN32")
ENDIF()
# here we instruct CMake to build STUDIO executable from all of the source files
ADD_EXECUTABLE( ShapeWorksStudio MACOSX_BUNDLE ${WinConsole}
  ${STUDIO_SRCS}
  ${STUDIO_HDRS}
  ${STUDIO_UI}
  ${STUDIO_ICNS}
  ${STUDIO_RSCS}
  )

IF(APPLE)
  SET_TARGET_PROPERTIES(ShapeWorksStudio PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Info.plist.in)
ENDIF(APPLE)
    
TARGET_LINK_LIBRARIES(ShapeWorksStudio
  Qt5::Widgets
  Qt5::OpenGL
  Qt5::Core
  Qt5::Gui
  ${VTK_LIBRARIES}
  vtkIOLegacy
  vtkRenderingQt
  vtkGUISupportQt
  vtkViewsQt
  ${ITK_LIBRARIES}
  ${OPENGL_LIBRARIES}
  ParticleSystemUtils
  tinyxml
  ShapeWorksGroom
  ShapeWorksOptimize
  Transforms
  SurfaceReconstruction
  ParticleSystemUtils
  Utils
  )

INSTALL(TARGETS ShapeWorksStudio
  BUNDLE  DESTINATION bin
  RUNTIME DESTINATION bin COMPONENT Runtime
  )
