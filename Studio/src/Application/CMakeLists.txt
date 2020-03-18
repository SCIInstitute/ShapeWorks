set(CMAKE_INCLUDE_CURRENT_DIR ON)
#set (CMAKE_AUTOMOC ON)

# Find QT5
find_package(OpenGL REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)

### Xlnt
#find_package (XLNT REQUIRED)

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

get_filename_component(Qt5_HOME "${Qt5_DIR}/../../.." REALPATH)


if(WIN32)
  find_program(WINDEPLOYQT_EXE
    HINTS ${Qt5_HOME} ${Qt5_HOME}/bin
    NAMES windeployqt.exe
    )
  if(WINDEPLOYQT_EXE MATCHES "NOTFOUND")
    message(FATAL_ERROR "windeployqt.exe is required to build the installer. Please ensure that it is installed and in your path. It should be found within the Qt5 install path.")
  endif()
  set(WINDEPLOYQT ${WINDEPLOYQT_EXE} CACHE INTERNAL "windeployqt executable")
  mark_as_advanced(WINDEPLOYQT)
endif()


FILE(GLOB STUDIO_UI  */*.ui)
QT5_WRAP_UI(UI_SRC ${STUDIO_UI})
FILE(GLOB UI_RESOURCES_RCC Resources/*.qrc)
QT5_ADD_RESOURCES(STUDIO_RSCS ${UI_RESOURCES_RCC})

#--------------------------------------------------------------------------------
### Application
#--------------------------------------------------------------------------------


FILE(GLOB STUDIO_DATA_SRCS Data/*.cpp)
FILE(GLOB STUDIO_DATA_HDRS Data/*.h)

FILE(GLOB STUDIO_ANALYSIS_SRCS Analysis/*.cpp)
FILE(GLOB STUDIO_ANALYSIS_HDRS Analysis/*.h)

FILE(GLOB STUDIO_GROOM_SRCS Groom/*.cpp)
FILE(GLOB STUDIO_GROOM_HDRS Groom/*.h)

FILE(GLOB STUDIO_OPTIMIZE_SRCS Optimize/*.cpp)
FILE(GLOB STUDIO_OPTIMIZE_HDRS Optimize/*.h)

FILE(GLOB STUDIO_VISUALIZATION_SRCS Visualization/*.cpp)
FILE(GLOB STUDIO_VISUALIZATION_HDRS Visualization/*.h)

SET(STUDIO_SRCS
  ${STUDIO_ANALYSIS_SRCS}
  ${STUDIO_DATA_SRCS}
  ${STUDIO_GROOM_SRCS}
  ${STUDIO_OPTIMIZE_SRCS}
  ${STUDIO_VISUALIZATION_SRCS}
)

SET(STUDIO_MOC_HDRS
  ${STUDIO_ANALYSIS_HDRS}
  ${STUDIO_DATA_HDRS}
  ${STUDIO_GROOM_HDRS}
  ${STUDIO_OPTIMIZE_HDRS}
  ${STUDIO_VISUALIZATION_HDRS}
)

qt5_wrap_cpp( STUDIO_MOC_SRCS ${STUDIO_MOC_HDRS} )




#FILE(GLOB STUDIO_SRCS main.cpp */*.cc */*.cpp */*.hxx)
#FILE(GLOB STUDIO_HDRS */*.h)
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
  # set the bundle identifier
  SET(MACOSX_BUNDLE_GUI_IDENTIFIER edu.utah.sci.shapeworks.studio)
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
  main.cpp
  ${STUDIO_SRCS}
  ${STUDIO_MOC_HDRS}
  ${STUDIO_MOC_SRCS}
  ${STUDIO_UI}
  ${STUDIO_ICNS}
  ${STUDIO_RSCS}
  )



if(WIN32)
  message(STATUS "Setting ${WINDEPLOYQT} command.")
  # start setting up install process by copying relevant DLLs
  add_custom_command(TARGET ShapeWorksStudio
    POST_BUILD
    # copy Qt libs
    COMMAND ${WINDEPLOYQT} ARGS --verbose 5 -core -widgets -gui -sql
                                --libdir $<TARGET_FILE_DIR:ShapeWorksStudio>
                                $<TARGET_FILE_DIR:ShapeWorksStudio>/ShapeWorksStudio.exe
  )
endif(WIN32)


IF(APPLE)
  SET_TARGET_PROPERTIES(ShapeWorksStudio PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Info.plist.in)
  set_property(
    TARGET ShapeWorksStudio
    PROPERTY INSTALL_RPATH
    "@executable_path/../../../../lib"
    "@executable_path/../Frameworks"
    )
ENDIF(APPLE)

TARGET_LINK_LIBRARIES(ShapeWorksStudio
  PUBLIC 
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
  tinyxml
  ShapeWorksGroom
  Alignment
  Analyze
  Optimize
  Utils
  xlnt::xlnt
  )

INSTALL(TARGETS ShapeWorksStudio
  BUNDLE  DESTINATION bin
  RUNTIME DESTINATION bin COMPONENT Runtime
  )

if (APPLE)
  # Add Qt frameworks to app bundle
  # see https://stackoverflow.com/questions/51556088/cmake-copy-dependencies-to-executable-output-path

  # Get reference to deployqt
  get_target_property(uic_location Qt5::uic IMPORTED_LOCATION)
  get_filename_component( _dir ${uic_location} DIRECTORY)
  set(deployqt "${_dir}/macdeployqt")
  if(NOT EXISTS ${deployqt})
    message(FATAL_ERROR "Failed to locate deployqt executable: [${deployqt}]")
  endif()

  # Execute deployqt during package creation
  # See https://doc.qt.io/qt-5/osx-deployment.html#macdeploy
  install(CODE "set(deployqt \"${deployqt}\")" COMPONENT Runtime)
  install(CODE [===[
    execute_process(COMMAND "${deployqt}" "${CMAKE_INSTALL_PREFIX}/bin/ShapeWorksStudio.app")
    ]===] COMPONENT Runtime)
endif()
