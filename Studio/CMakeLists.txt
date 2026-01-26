set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

IF (WIN32 AND MSVC)
  ADD_DEFINITIONS(-D_SCL_SECURE_NO_WARNINGS)
  ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
ENDIF (WIN32 AND MSVC)

# Find QT5
find_package(OpenGL REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(Qt5Network REQUIRED)

cmake_policy(SET CMP0020 NEW)

#headers
include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5OpenGL_INCLUDE_DIRS})
include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Gui_INCLUDE_DIRS})
include_directories(${Qt5Xml_INCLUDE_DIRS})
include_directories(${Qt5PrintSupport_INCLUDE_DIRS})

#defines
add_definitions(${Qt5Widgets_DEFINITIONS})
add_definitions(${Qt5OpenGL_DEFINITIONS})
add_definitions(${Qt5Core_DEFINITIONS})
add_definitions(${Qt5Gui_DEFINITIONS})
add_definitions(${Qt5Xml_DEFINITIONS})
add_definitions(${Qt5PrintSupport_DEFINITIONS})

get_target_property(QtWidgets_location Qt5::Widgets LOCATION)
get_target_property(QtGui_location Qt5::Gui LOCATION)
get_target_property(QtCore_location Qt5::Core LOCATION)
get_target_property(QtOpenGL_location Qt5::OpenGL LOCATION)
get_target_property(QtPrintSupport_location Qt5::PrintSupport LOCATION)
get_target_property(QtXml_location Qt5::Gui LOCATION)

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


SET(STUDIO_DATA_SRCS
  Data/LandmarkItemDelegate.cpp
  Data/LandmarkTableModel.cpp
  Data/DataTool.cpp
  Data/ExportUtils.cpp
  Data/MeshSettings.cpp
  Data/Preferences.cpp
  Data/PreferencesWindow.cpp
  Data/Session.cpp
  Data/ShapeWorksWorker.cpp
  Data/Telemetry.cpp
  Data/Worker.cpp
  Data/SegmentationToolPanel.cpp
  )

SET(STUDIO_DATA_MOC_HDRS
  Data/LandmarkItemDelegate.h
  Data/LandmarkTableModel.h
  Data/DataTool.h
  Data/Preferences.h
  Data/PreferencesWindow.h
  Data/Session.h
  Data/ShapeWorksWorker.h
  Data/Telemetry.h
  Data/Worker.h
  Data/SegmentationToolPanel.h
  )

SET(STUDIO_JOB_SRCS
  Job/GroupPvalueJob.cpp
  Job/ParticleAreaJob.cpp
  Job/NetworkAnalysisJob.cpp
  Job/ParticleNormalEvaluationJob.cpp
  Job/StatsGroupLDAJob.cpp
  Job/ShapeScalarJob.cpp
  )

SET(STUDIO_JOB_MOC_HDRS
  Job/GroupPvalueJob.h
  Job/NetworkAnalysisJob.h
  Job/ParticleAreaJob.h
  Job/ParticleNormalEvaluationJob.h
  Job/StatsGroupLDAJob.h
  Job/ShapeScalarJob.h
  )

SET(STUDIO_GROOM_SRCS
  Groom/GroomTool.cpp
  )
SET(STUDIO_GROOM_MOC_HDRS
  Groom/GroomTool.h
  )

SET(STUDIO_OPTIMIZE_SRCS
  Optimize/OptimizeTool.cpp
  Optimize/QOptimize.cpp
  )
SET(STUDIO_OPTIMIZE_MOC_HDRS
  Optimize/OptimizeTool.h
  Optimize/QOptimize.h
  )

SET(STUDIO_ANALYSIS_SRCS
  Analysis/AnalysisTool.cpp
  Analysis/ParticleAreaPanel.cpp
  Analysis/ShapeEvaluationJob.cpp
  Analysis/ShapeScalarPanel.cpp
  )
SET(STUDIO_ANALYSIS_MOC_HDRS
  Analysis/AnalysisTool.h
  Analysis/ParticleAreaPanel.h
  Analysis/ShapeEvaluationJob.h
  Analysis/ShapeScalarPanel.h
  )

SET(STUDIO_DEEPSSM_SRCS
  DeepSSM/DeepSSMTool.cpp
  )
SET(STUDIO_DEEPSSM_MOC_HDRS
  DeepSSM/DeepSSMTool.h
  )


SET(STUDIO_MONAILABEL_SRCS
ShapeWorksMONAI/MonaiLabelTool.cpp
ShapeWorksMONAI/MonaiLabelJob.cpp
ShapeWorksMONAI/MonaiLabelUtils.cpp
)

SET(STUDIO_MONAILABEL_MOC_HDRS
  ShapeWorksMONAI/MonaiLabelTool.h
  ShapeWorksMONAI/MonaiLabelJob.h
  )

SET(STUDIO_UTILS_SRCS
  Utils/StackWalker.cpp
  Utils/WindowsCrashHandler.cpp
  Utils/StudioUtils.cpp
  Utils/AnalysisUtils.cpp
  Utils/ImageLabel.cpp
  )

SET(STUDIO_UTILS_MOC_HDRS
  # Utils/ImageLabel.h - doesn't have Q_OBJECT, no moc needed
  )

SET(STUDIO_INTERFACE_SRCS
  Interface/CompareWidget.cpp
  Interface/CustomSlider.cpp
  Interface/ExportImageDialog.cpp
  Interface/KeyboardShortcuts.cpp
  Interface/LogWindow.cpp
  Interface/ShapeWorksStudioApp.cpp
  Interface/SplashScreen.cpp
  Interface/StatusBarWidget.cpp
  Interface/StudioLogger.cpp
  Interface/Style.cpp
  Interface/UpdateChecker.cpp
  Interface/WheelEventForwarder.cpp
  )
SET(STUDIO_INTERFACE_MOC_HDRS
  Interface/CompareWidget.h
  # Interface/CustomSlider.h - doesn't have Q_OBJECT, no moc needed
  Interface/ExportImageDialog.h
  Interface/KeyboardShortcuts.h
  Interface/LogWindow.h
  Interface/ShapeWorksStudioApp.h
  Interface/SplashScreen.h
  Interface/StatusBarWidget.h
  Interface/StudioLogger.h
  Interface/UpdateChecker.h
  Interface/WheelEventForwarder.h
  )

SET(STUDIO_VISUALIZATION_SRCS
  Visualization/ColorMap.cpp
  Visualization/Lightbox.cpp
  Visualization/LandmarkWidget.cpp
  Visualization/PlaneWidget.cpp
  Visualization/PaintWidget.cpp
  Visualization/ParticleColors.cpp
  Visualization/StudioInteractorStyle.cpp
  Visualization/StudioSliceInteractorStyle.cpp
  Visualization/StudioImageActorPointPlacer.cpp
  Visualization/SliceView.cpp
  Visualization/MeshSlice.cpp
  Visualization/Viewer.cpp
  Visualization/Visualizer.cpp
  Visualization/StudioHandleWidget.cpp
)

SET(STUDIO_VISUALIZATION_MOC_HDRS
  Visualization/Lightbox.h
  Visualization/ParticleColors.h
  Visualization/Visualizer.h
)

SET(STUDIO_SRCS
  ${STUDIO_ANALYSIS_SRCS}
  ${STUDIO_DATA_SRCS}
  ${STUDIO_JOB_SRCS}
  ${STUDIO_DEEPSSM_SRCS}
  ${STUDIO_MONAILABEL_SRCS}
  ${STUDIO_GROOM_SRCS}
  ${STUDIO_INTERFACE_SRCS}
  ${STUDIO_OPTIMIZE_SRCS}
  ${STUDIO_UTILS_SRCS}
  ${STUDIO_VISUALIZATION_SRCS}
)

SET(STUDIO_MOC_HDRS
  ${STUDIO_ANALYSIS_MOC_HDRS}
  ${STUDIO_DATA_MOC_HDRS}
  ${STUDIO_JOB_MOC_HDRS}
  ${STUDIO_DEEPSSM_MOC_HDRS}
  ${STUDIO_MONAILABEL_MOC_HDRS}
  ${STUDIO_GROOM_MOC_HDRS}
  ${STUDIO_INTERFACE_MOC_HDRS}
  ${STUDIO_OPTIMIZE_MOC_HDRS}
  ${STUDIO_UTILS_MOC_HDRS}
  ${STUDIO_VISUALIZATION_MOC_HDRS}
)

qt5_wrap_cpp( STUDIO_MOC_SRCS ${STUDIO_MOC_HDRS} )


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
  # set the version ($SHAPEWORKS_VERSION)
  SET(MACOSX_BUNDLE_SHORT_VERSION_STRING ${SHAPEWORKS_VERSION})
  SET(MACOSX_BUNDLE_VERSION_STRING_STRING ${SHAPEWORKS_VERSION})

  # set where in the bundle to put the icns file
  SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/Resources/ShapeWorksStudio.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  # include the icns file in the target
  SET(STUDIO_SRCS ${STUDIO_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/Resources/ShapeWorksStudio.icns)


ELSEIF(WIN32)
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
    COMMAND ${WINDEPLOYQT} ARGS --verbose 5 -core -widgets -gui -sql -xml -printsupport --no-translations
                                --libdir $<TARGET_FILE_DIR:ShapeWorksStudio>
                                $<TARGET_FILE_DIR:ShapeWorksStudio>/ShapeWorksStudio.exe
  )

  # install msvcp140.dll and vcruntime140.dll
  file(TO_CMAKE_PATH "$ENV{windir}/SysWOW64" WIN_SYSDIR)
  if (NOT EXISTS "${WIN_SYSDIR}")
    file(TO_CMAKE_PATH "$ENV{windir}/system32" WIN_SYSDIR)
  endif()
  install(FILES "${WIN_SYSDIR}/msvcp140.dll" "${WIN_SYSDIR}/vcomp140.dll" "${WIN_SYSDIR}/vcruntime140.dll" "${WIN_SYSDIR}/vcruntime140_1.dll" DESTINATION bin)
    
endif(WIN32)


IF(APPLE)

  # Create a script that runs ShapeWorksStudio.  Note that a symlink won't work as it won't be able to find the property list
  SET(SCRIPT "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/script_tmp/ShapeWorksStudio")
  file( WRITE  ${SCRIPT} "#!/bin/bash\n")
  file( APPEND ${SCRIPT} "LOC=\"`dirname \"$0\"`\"\n" )
  file( APPEND ${SCRIPT} "\${LOC}/ShapeWorksStudio.app/Contents/MacOS/ShapeWorksStudio \"$@\"\n" )
  MESSAGE(STATUS "Creating ShapeWorksStudio script in ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" )
  file( COPY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/script_tmp/ShapeWorksStudio" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" FILE_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ)
  file( REMOVE_RECURSE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/script_tmp")

  INSTALL(PROGRAMS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ShapeWorksStudio" DESTINATION bin)
  
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
  Qt5::Xml
  Qt5::PrintSupport
  Qt5::Network
  ${VTK_LIBRARIES}
  ${ITK_LIBRARIES}
  ${OPENGL_LIBRARIES}
  tinyxml
  Application
  Alignment
  Analyze
  OptimizeLibraries
  Groom
  Utils
  Project
  xlnt::xlnt
  Eigen3::Eigen
  JKQTPlotterSharedLib
  )

IF(USE_PCH)
  target_precompile_headers(ShapeWorksStudio PRIVATE
    "<vtkPolyData.h>"
    "<itkImage.h>"
    "<itkAffineTransform.h>"
    "<itkStatisticsImageFilter.h>"
    "<Eigen/Dense>"
    "<Eigen/Sparse>"
    "<Eigen/Core>"
    "<QCloseEvent>"
    "<QFileDialog>"
    )
ENDIF(USE_PCH)

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
    execute_process(COMMAND "${deployqt}" "${CMAKE_INSTALL_PREFIX}/bin/ShapeWorksStudio.app" "-no-strip" "-verbose=3")
    ]===] COMPONENT Runtime)
endif()
