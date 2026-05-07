# InstallBundledPython.cmake — Install rules for packaging bundled Python into the .app
#
# Installs the bundled Python into the macOS .app bundle at:
#   bin/ShapeWorksStudio.app/Contents/Resources/Python/
#
# This file is included from Studio/CMakeLists.txt AFTER macdeployqt runs,
# and only takes effect when USE_BUNDLED_PYTHON is ON.

if(NOT USE_BUNDLED_PYTHON)
  return()
endif()

if(NOT APPLE)
  message(STATUS "InstallBundledPython: skipping (not macOS)")
  return()
endif()

set(_app_python_dest "bin/ShapeWorksStudio.app/Contents/Resources/Python")

# ---------------------------------------------------------------------------
# 0. Clean previous bundled Python to avoid stale files from old package versions
# ---------------------------------------------------------------------------
install(CODE "
  set(_old_python \"${CMAKE_INSTALL_PREFIX}/${_app_python_dest}\")
  if(IS_DIRECTORY \"\${_old_python}\")
    message(STATUS \"Removing previous bundled Python at \${_old_python}\")
    file(REMOVE_RECURSE \"\${_old_python}\")
  endif()
" COMPONENT Runtime)

# ---------------------------------------------------------------------------
# 1. Install Python standard library (excluding bloat)
# ---------------------------------------------------------------------------
install(DIRECTORY "${BUNDLED_PYTHON_ROOT}/lib/python3.12/"
  DESTINATION "${_app_python_dest}/lib/python3.12"
  PATTERN "__pycache__" EXCLUDE
  PATTERN "test" EXCLUDE
  PATTERN "tests" EXCLUDE
  PATTERN "tkinter" EXCLUDE
  PATTERN "ensurepip" EXCLUDE
  PATTERN "idlelib" EXCLUDE
  PATTERN "turtledemo" EXCLUDE
  PATTERN "turtle.py" EXCLUDE
  PATTERN "lib2to3" EXCLUDE
)

# ---------------------------------------------------------------------------
# 2. Install libpython shared library
# ---------------------------------------------------------------------------
file(GLOB _libpython_dylibs "${BUNDLED_PYTHON_ROOT}/lib/libpython3.12*.dylib")
install(FILES ${_libpython_dylibs}
  DESTINATION "${_app_python_dest}/lib"
)

# Also replace the Frameworks copy (placed by macdeployqt from conda) with the
# bundled version so that sys.version matches the bundled stdlib.
install(CODE "
  set(_fw_libpython \"${CMAKE_INSTALL_PREFIX}/bin/ShapeWorksStudio.app/Contents/Frameworks/libpython3.12.dylib\")
  set(_bundled_libpython \"${BUNDLED_PYTHON_ROOT}/lib/libpython3.12.dylib\")
  if(EXISTS \"\${_fw_libpython}\" AND EXISTS \"\${_bundled_libpython}\")
    file(REAL_PATH \"\${_bundled_libpython}\" _real_bundled)
    message(STATUS \"Replacing Frameworks libpython with bundled version\")
    file(COPY \"\${_real_bundled}\" DESTINATION
      \"${CMAKE_INSTALL_PREFIX}/bin/ShapeWorksStudio.app/Contents/Frameworks\")
    get_filename_component(_real_name \"\${_real_bundled}\" NAME)
    if(NOT \"\${_real_name}\" STREQUAL \"libpython3.12.dylib\")
      file(RENAME
        \"${CMAKE_INSTALL_PREFIX}/bin/ShapeWorksStudio.app/Contents/Frameworks/\${_real_name}\"
        \"\${_fw_libpython}\")
    endif()
    execute_process(COMMAND install_name_tool -id \"@rpath/libpython3.12.dylib\" \"\${_fw_libpython}\")
  endif()
" COMPONENT Runtime)

# ---------------------------------------------------------------------------
# 3. Install python3 executable (needed for multiprocessing)
# ---------------------------------------------------------------------------
install(PROGRAMS "${BUNDLED_PYTHON_ROOT}/bin/python3"
  DESTINATION "${_app_python_dest}/bin"
)
# Also install python3.12 symlink/binary if present
if(EXISTS "${BUNDLED_PYTHON_ROOT}/bin/python3.12")
  install(PROGRAMS "${BUNDLED_PYTHON_ROOT}/bin/python3.12"
    DESTINATION "${_app_python_dest}/bin"
  )
endif()

# ---------------------------------------------------------------------------
# 4. Install ShapeWorks Python packages into site-packages
# ---------------------------------------------------------------------------
set(_site_packages "${_app_python_dest}/lib/python3.12/site-packages")

# Core shapeworks package
install(DIRECTORY "${CMAKE_SOURCE_DIR}/Python/shapeworks/shapeworks"
  DESTINATION "${_site_packages}"
  PATTERN "__pycache__" EXCLUDE
  PATTERN "*.pyc" EXCLUDE
)

# Additional ShapeWorks Python packages
set(_sw_python_packages
  DataAugmentationUtilsPackage/DataAugmentationUtils
  DatasetUtilsPackage/DatasetUtils
  DeepSSMUtilsPackage/DeepSSMUtils
  DocumentationUtilsPackage/DocumentationUtils
  ShapeCohortGenPackage/ShapeCohortGen
)

foreach(_pkg ${_sw_python_packages})
  get_filename_component(_pkg_name "${_pkg}" NAME)
  get_filename_component(_pkg_parent "${_pkg}" DIRECTORY)
  if(EXISTS "${CMAKE_SOURCE_DIR}/Python/${_pkg}")
    install(DIRECTORY "${CMAKE_SOURCE_DIR}/Python/${_pkg}"
      DESTINATION "${_site_packages}"
      PATTERN "__pycache__" EXCLUDE
      PATTERN "*.pyc" EXCLUDE
    )
  endif()
endforeach()

# ---------------------------------------------------------------------------
# 5. Install shapeworks_py pybind11 module into site-packages
# ---------------------------------------------------------------------------
install(CODE "
  file(GLOB _pybind_module \"${CMAKE_INSTALL_PREFIX}/bin/shapeworks_py*.so\")
  if(_pybind_module)
    list(GET _pybind_module 0 _pybind_src)
    get_filename_component(_pybind_name \"\${_pybind_src}\" NAME)
    file(INSTALL \"\${_pybind_src}\"
      DESTINATION \"${CMAKE_INSTALL_PREFIX}/${_site_packages}\"
    )
    message(STATUS \"Installed \${_pybind_name} into bundled site-packages\")
    # Add @loader_path RPATH so shapeworks_py.so can find libraries in Frameworks
    # when loaded by the bundled python3 (multiprocessing subprocesses).
    # From site-packages/, 5 levels up reaches Contents/, then into Frameworks/.
    execute_process(
      COMMAND install_name_tool -add_rpath \"@loader_path/../../../../../Frameworks\"
        \"${CMAKE_INSTALL_PREFIX}/${_site_packages}/\${_pybind_name}\"
    )
    message(STATUS \"Added Frameworks RPATH to \${_pybind_name}\")
  else()
    message(WARNING \"shapeworks_py module not found in ${CMAKE_INSTALL_PREFIX}/bin/\")
  endif()
" COMPONENT Runtime)

# ---------------------------------------------------------------------------
# 6. Fix libpython install name to use @rpath
# ---------------------------------------------------------------------------
install(CODE "
  file(GLOB _libpython_files \"${CMAKE_INSTALL_PREFIX}/${_app_python_dest}/lib/libpython3.12*.dylib\")
  foreach(_lib \${_libpython_files})
    get_filename_component(_lib_name \"\${_lib}\" NAME)
    execute_process(
      COMMAND install_name_tool -id \"@rpath/\${_lib_name}\" \"\${_lib}\"
    )
    message(STATUS \"Fixed install name for \${_lib_name}\")
  endforeach()
" COMPONENT Runtime)

message(STATUS "InstallBundledPython: will install bundled Python to ${_app_python_dest}")
