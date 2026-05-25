# InstallBundledPython.cmake — Install rules for packaging bundled Python
#
# On macOS: Installs into bin/ShapeWorksStudio.app/Contents/Resources/Python/
# On Linux: Installs into lib/python/
# On Windows: Installs into lib/python/ with DLLs copied to bin/
#
# This file is included from Studio/CMakeLists.txt AFTER platform-specific
# deploy steps, and only takes effect when USE_BUNDLED_PYTHON is ON.

if(NOT USE_BUNDLED_PYTHON)
  return()
endif()

# ---------------------------------------------------------------------------
# Common: ShapeWorks Python packages to install into site-packages
# ---------------------------------------------------------------------------
set(_sw_python_packages
  DataAugmentationUtilsPackage/DataAugmentationUtils
  DatasetUtilsPackage/DatasetUtils
  DeepSSMUtilsPackage/DeepSSMUtils
  DocumentationUtilsPackage/DocumentationUtils
  ShapeCohortGenPackage/ShapeCohortGen
)

# ###########################################################################
# Windows install rules (separate section — layout differs significantly)
# ###########################################################################
if(WIN32)

set(_python_dest "lib/python")

# ---------------------------------------------------------------------------
# 0. Clean previous bundled Python to avoid stale files
# ---------------------------------------------------------------------------
install(CODE "
  set(_old_python \"${CMAKE_INSTALL_PREFIX}/${_python_dest}\")
  if(IS_DIRECTORY \"\${_old_python}\")
    message(STATUS \"Removing previous bundled Python at \${_old_python}\")
    file(REMOVE_RECURSE \"\${_old_python}\")
  endif()
" COMPONENT Runtime)

# ---------------------------------------------------------------------------
# 1. Install Python standard library (Lib/ on Windows, excluding bloat)
# ---------------------------------------------------------------------------
install(DIRECTORY "${BUNDLED_PYTHON_ROOT}/Lib/"
  DESTINATION "${_python_dest}/Lib"
  PATTERN "__pycache__" EXCLUDE
  PATTERN "test" EXCLUDE
  PATTERN "tests" EXCLUDE
  PATTERN "tkinter" EXCLUDE
  PATTERN "ensurepip" EXCLUDE
  PATTERN "idlelib" EXCLUDE
  PATTERN "turtledemo" EXCLUDE
  PATTERN "turtle.py" EXCLUDE
  PATTERN "lib2to3" EXCLUDE
  PATTERN "*.egg-link" EXCLUDE
  PATTERN "__editable__*" EXCLUDE
  PATTERN "easy-install.pth" EXCLUDE
)

# ---------------------------------------------------------------------------
# 2. Install DLLs directory (C extension modules + dependency DLLs)
# ---------------------------------------------------------------------------
install(DIRECTORY "${BUNDLED_PYTHON_ROOT}/DLLs/"
  DESTINATION "${_python_dest}/DLLs"
  PATTERN "__pycache__" EXCLUDE
  PATTERN "_test*" EXCLUDE
)

# ---------------------------------------------------------------------------
# 3. Install python.exe (needed for multiprocessing and on-demand installs)
# ---------------------------------------------------------------------------
install(PROGRAMS "${BUNDLED_PYTHON_ROOT}/python.exe"
  DESTINATION "${_python_dest}"
)
if(EXISTS "${BUNDLED_PYTHON_ROOT}/pythonw.exe")
  install(PROGRAMS "${BUNDLED_PYTHON_ROOT}/pythonw.exe"
    DESTINATION "${_python_dest}"
  )
endif()

# ---------------------------------------------------------------------------
# 4. Install python DLLs — python312.dll and python3.dll to both bin/ and lib/python/
#    They must be in bin/ so the main executables find them, and in lib/python/
#    so the bundled python.exe finds them.
# ---------------------------------------------------------------------------
install(FILES
  "${BUNDLED_PYTHON_ROOT}/python312.dll"
  "${BUNDLED_PYTHON_ROOT}/python3.dll"
  DESTINATION bin
)
install(FILES
  "${BUNDLED_PYTHON_ROOT}/python312.dll"
  "${BUNDLED_PYTHON_ROOT}/python3.dll"
  DESTINATION "${_python_dest}"
)
# Also install vcruntime from the bundled Python (may differ from system)
foreach(_vcrt vcruntime140.dll vcruntime140_1.dll)
  if(EXISTS "${BUNDLED_PYTHON_ROOT}/${_vcrt}")
    install(FILES "${BUNDLED_PYTHON_ROOT}/${_vcrt}" DESTINATION bin)
    install(FILES "${BUNDLED_PYTHON_ROOT}/${_vcrt}" DESTINATION "${_python_dest}")
  endif()
endforeach()

# Copy critical dependency DLLs from DLLs/ to bin/ so they are findable by the
# embedded interpreter. When python312.dll is loaded inside ShapeWorksStudio.exe,
# .pyd extensions (like _ctypes.pyd) need their implicit DLL dependencies
# (like libffi-8.dll) on the DLL search path. Placing them in bin/ (next to the
# host executable) ensures they are always found.
foreach(_dll libffi-8.dll libcrypto-3-x64.dll libssl-3-x64.dll sqlite3.dll)
  if(EXISTS "${BUNDLED_PYTHON_ROOT}/DLLs/${_dll}")
    install(FILES "${BUNDLED_PYTHON_ROOT}/DLLs/${_dll}" DESTINATION bin)
  endif()
endforeach()

# ---------------------------------------------------------------------------
# 5. Install ShapeWorks Python packages into site-packages
# ---------------------------------------------------------------------------
set(_site_packages "${_python_dest}/Lib/site-packages")

install(DIRECTORY "${CMAKE_SOURCE_DIR}/Python/shapeworks/shapeworks"
  DESTINATION "${_site_packages}"
  PATTERN "__pycache__" EXCLUDE
  PATTERN "*.pyc" EXCLUDE
)

foreach(_pkg ${_sw_python_packages})
  if(EXISTS "${CMAKE_SOURCE_DIR}/Python/${_pkg}")
    install(DIRECTORY "${CMAKE_SOURCE_DIR}/Python/${_pkg}"
      DESTINATION "${_site_packages}"
      PATTERN "__pycache__" EXCLUDE
      PATTERN "*.pyc" EXCLUDE
    )
  endif()
endforeach()

# ---------------------------------------------------------------------------
# 6. Install shapeworks_py pybind11 module (.pyd) into site-packages
# ---------------------------------------------------------------------------
install(CODE "
  file(GLOB _pybind_module \"${CMAKE_INSTALL_PREFIX}/bin/shapeworks_py*.pyd\")
  if(_pybind_module)
    list(GET _pybind_module 0 _pybind_src)
    get_filename_component(_pybind_name \"\${_pybind_src}\" NAME)
    file(INSTALL \"\${_pybind_src}\"
      DESTINATION \"${CMAKE_INSTALL_PREFIX}/${_site_packages}\"
    )
    message(STATUS \"Installed \${_pybind_name} into bundled site-packages\")
  else()
    message(WARNING \"shapeworks_py module not found in ${CMAKE_INSTALL_PREFIX}/bin/\")
  endif()
" COMPONENT Runtime)

# ---------------------------------------------------------------------------
# 6b. Install shared VTK DLLs to bin/ (when VTK was built shared)
# ---------------------------------------------------------------------------
set(_vtk_lib_dir "")
foreach(_pp ${CMAKE_PREFIX_PATH})
  if(EXISTS "${_pp}/bin/vtkCommonCore-9.5.dll")
    set(_vtk_lib_dir "${_pp}/bin")
  endif()
endforeach()
if(_vtk_lib_dir)
  install(CODE "
    file(GLOB _vtk_dlls \"${_vtk_lib_dir}/vtk*.dll\")
    foreach(_dll \${_vtk_dlls})
      get_filename_component(_name \"\${_dll}\" NAME)
      file(INSTALL \"\${_dll}\" DESTINATION \"${CMAKE_INSTALL_PREFIX}/bin\")
    endforeach()
    list(LENGTH _vtk_dlls _n)
    message(STATUS \"Installed \${_n} VTK DLLs to bin/\")
  " COMPONENT Runtime)
endif()

# Install 'swpython.bat' wrapper that launches the bundled Python with
# PYTHONHOME set, and prepends the bundle bin/ to PATH so use case scripts
# can find the shapeworks CLI via shutil.which.
set(_swpython_dir "${CMAKE_BINARY_DIR}/_bundled_python/swpython")
file(MAKE_DIRECTORY "${_swpython_dir}")
file(WRITE "${_swpython_dir}/swpython.bat"
[=[@echo off
set "LOC=%~dp0"
set "PYTHONHOME=%LOC%..\lib\python"
set "PATH=%LOC%;%PATH%"
set "PYTHONPATH="
if not defined MPLBACKEND set "MPLBACKEND=Agg"
"%PYTHONHOME%\python.exe" %*
]=])
install(PROGRAMS "${_swpython_dir}/swpython.bat" DESTINATION bin)

# swpip.bat — pip wrapper that delegates to swpython.bat -m shapeworks.swpip
set(_swpip_dir "${CMAKE_BINARY_DIR}/_bundled_python/swpip")
file(MAKE_DIRECTORY "${_swpip_dir}")
file(WRITE "${_swpip_dir}/swpip.bat"
[=[@echo off
set "LOC=%~dp0"
call "%LOC%swpython.bat" -m shapeworks.swpip %*
]=])
install(PROGRAMS "${_swpip_dir}/swpip.bat" DESTINATION bin)

message(STATUS "InstallBundledPython: will install bundled Python to ${_python_dest}")
return()
endif() # WIN32

# ###########################################################################
# macOS + Linux install rules (unified with platform-specific variables)
# ###########################################################################

# ---------------------------------------------------------------------------
# Platform-specific paths
# ---------------------------------------------------------------------------
if(APPLE)
  set(_app_python_dest "bin/ShapeWorksStudio.app/Contents/Resources/Python")
  set(_libpython_glob "libpython3.12*.dylib")
  set(_pybind_glob "shapeworks_py*.so")
else()
  # Linux: install Python alongside the lib/ directory
  set(_app_python_dest "lib/python")
  set(_libpython_glob "libpython3.12*.so*")
  set(_pybind_glob "shapeworks_py*.so")
endif()

set(_site_packages "${_app_python_dest}/lib/python3.12/site-packages")

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
  # Editable-install markers left by `pip install -e` during bundled_pip_install
  # — pollute the deployed bundle and only point at the build machine's source tree.
  PATTERN "*.egg-link" EXCLUDE
  PATTERN "__editable__*" EXCLUDE
  PATTERN "easy-install.pth" EXCLUDE
)

# ---------------------------------------------------------------------------
# 2. Install libpython shared library
# ---------------------------------------------------------------------------
file(GLOB _libpython_files "${BUNDLED_PYTHON_ROOT}/lib/${_libpython_glob}")
install(FILES ${_libpython_files}
  DESTINATION "${_app_python_dest}/lib"
)

if(APPLE)
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
else()
  # Linux: Also copy libpython to lib/ (alongside other shared libraries)
  # so executables can find it via $ORIGIN/../lib RPATH
  install(CODE "
    set(_destdir \"\$ENV{DESTDIR}\")
    set(_prefix \"${CMAKE_INSTALL_PREFIX}\")
    set(_install_root \"\${_destdir}\${_prefix}\")
    file(GLOB _libpython_so \"${BUNDLED_PYTHON_ROOT}/lib/libpython3.12.so*\")
    foreach(_lib \${_libpython_so})
      file(REAL_PATH \"\${_lib}\" _real_lib)
      get_filename_component(_lib_name \"\${_lib}\" NAME)
      message(STATUS \"Installing \${_lib_name} to lib/\")
      file(COPY \"\${_real_lib}\" DESTINATION \"\${_install_root}/lib\")
      get_filename_component(_real_name \"\${_real_lib}\" NAME)
      if(NOT \"\${_real_name}\" STREQUAL \"\${_lib_name}\")
        file(RENAME \"\${_install_root}/lib/\${_real_name}\" \"\${_install_root}/lib/\${_lib_name}\")
      endif()
    endforeach()
  " COMPONENT Runtime)
endif()

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

# Core shapeworks package
install(DIRECTORY "${CMAKE_SOURCE_DIR}/Python/shapeworks/shapeworks"
  DESTINATION "${_site_packages}"
  PATTERN "__pycache__" EXCLUDE
  PATTERN "*.pyc" EXCLUDE
)

foreach(_pkg ${_sw_python_packages})
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
if(APPLE)
  install(CODE "
    set(_destdir \"\$ENV{DESTDIR}\")
    set(_prefix \"${CMAKE_INSTALL_PREFIX}\")
    set(_install_root \"\${_destdir}\${_prefix}\")
    file(GLOB _pybind_module \"\${_install_root}/bin/${_pybind_glob}\")
    if(_pybind_module)
      list(GET _pybind_module 0 _pybind_src)
      get_filename_component(_pybind_name \"\${_pybind_src}\" NAME)
      file(COPY \"\${_pybind_src}\"
        DESTINATION \"\${_install_root}/${_site_packages}\"
      )
      message(STATUS \"Installed \${_pybind_name} into bundled site-packages\")
      # Add @loader_path RPATH so shapeworks_py.so can find libraries in Frameworks
      # when loaded by the bundled python3 (multiprocessing subprocesses).
      # From site-packages/, 5 levels up reaches Contents/, then into Frameworks/.
      execute_process(
        COMMAND install_name_tool -add_rpath \"@loader_path/../../../../../Frameworks\"
          \"\${_install_root}/${_site_packages}/\${_pybind_name}\"
      )
      message(STATUS \"Added Frameworks RPATH to \${_pybind_name}\")
    else()
      message(WARNING \"shapeworks_py module not found in \${_install_root}/bin/\")
    endif()
  " COMPONENT Runtime)
else()
  # Linux: copy shapeworks_py.so and set RPATH using patchelf
  install(CODE "
    set(_destdir \"\$ENV{DESTDIR}\")
    set(_prefix \"${CMAKE_INSTALL_PREFIX}\")
    set(_install_root \"\${_destdir}\${_prefix}\")
    file(GLOB _pybind_module \"\${_install_root}/bin/${_pybind_glob}\")
    if(_pybind_module)
      list(GET _pybind_module 0 _pybind_src)
      get_filename_component(_pybind_name \"\${_pybind_src}\" NAME)
      file(COPY \"\${_pybind_src}\"
        DESTINATION \"\${_install_root}/${_site_packages}\"
      )
      message(STATUS \"Installed \${_pybind_name} into bundled site-packages\")
      # Set RPATH so shapeworks_py.so can find libraries in lib/
      # From site-packages/, we need to go up to lib/python/, then to lib/
      # site-packages -> python3.12 -> lib -> python -> lib (where .so files are)
      find_program(_patchelf patchelf)
      if(_patchelf)
        execute_process(
          COMMAND \${_patchelf} --set-rpath \"\$ORIGIN/../../../../lib:\$ORIGIN/../../../../../lib\"
            \"\${_install_root}/${_site_packages}/\${_pybind_name}\"
        )
        message(STATUS \"Set RPATH on \${_pybind_name}\")
      else()
        message(WARNING \"patchelf not found — shapeworks_py.so RPATH not set\")
      endif()
    else()
      message(WARNING \"shapeworks_py module not found in \${_install_root}/bin/\")
    endif()
  " COMPONENT Runtime)
endif()

# ---------------------------------------------------------------------------
# 6. Fix library install names/RPATH
# ---------------------------------------------------------------------------
if(APPLE)
  install(CODE "
    set(_destdir \"\$ENV{DESTDIR}\")
    set(_prefix \"${CMAKE_INSTALL_PREFIX}\")
    set(_install_root \"\${_destdir}\${_prefix}\")
    file(GLOB _libpython_files \"\${_install_root}/${_app_python_dest}/lib/libpython3.12*.dylib\")
    foreach(_lib \${_libpython_files})
      get_filename_component(_lib_name \"\${_lib}\" NAME)
      execute_process(
        COMMAND install_name_tool -id \"@rpath/\${_lib_name}\" \"\${_lib}\"
      )
      message(STATUS \"Fixed install name for \${_lib_name}\")
    endforeach()
  " COMPONENT Runtime)
else()
  # Linux: Set RPATH on the bundled python3 executable
  install(CODE "
    set(_destdir \"\$ENV{DESTDIR}\")
    set(_prefix \"${CMAKE_INSTALL_PREFIX}\")
    set(_install_root \"\${_destdir}\${_prefix}\")
    set(_python_exe \"\${_install_root}/${_app_python_dest}/bin/python3\")
    if(EXISTS \"\${_python_exe}\")
      find_program(_patchelf patchelf)
      if(_patchelf)
        # python3 needs to find libpython in ../lib (relative to bin/)
        execute_process(
          COMMAND \${_patchelf} --set-rpath \"\$ORIGIN/../lib\" \"\${_python_exe}\"
        )
        message(STATUS \"Set RPATH on bundled python3\")
      endif()
    endif()
  " COMPONENT Runtime)
endif()

# ---------------------------------------------------------------------------
# 6b. Shared VTK: bundle the shared libvtk* and point the dep-built vtkmodules
#     at them. Only when VTK was built shared (libvtk*.dylib/.so present in the
#     dep lib dir); a static VTK build leaves these globs empty and skips this.
#     The vtkmodules + stub vtk dist-info are already in site-packages (copied
#     with the bundled Python tree, provisioned by bundled_pip_install).
# ---------------------------------------------------------------------------
get_filename_component(_vtk_lib_dir "${VTK_DIR}/../.." ABSOLUTE)
if(APPLE)
  file(GLOB _vtk_shared_check "${_vtk_lib_dir}/libvtk*.dylib")
  if(_vtk_shared_check)
    install(CODE "
      set(_install_root \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}\")
      set(_fw \"\${_install_root}/bin/ShapeWorksStudio.app/Contents/Frameworks\")
      file(MAKE_DIRECTORY \"\${_fw}\")
      # cp -a preserves the libvtkX-9.5.dylib -> -9.5.1 -> -9.5.9.5 symlink chain
      execute_process(COMMAND bash -c \"cp -a '${_vtk_lib_dir}'/libvtk*.dylib '\${_fw}/'\")
      message(STATUS \"Bundled shared VTK dylibs into Frameworks\")
      # vtkmodules .so reference @rpath/libvtk*; add a Frameworks rpath. From
      # site-packages/vtkmodules/ to Contents/Frameworks is 6 levels up.
      file(GLOB _vtkmods \"\${_install_root}/${_site_packages}/vtkmodules/*.so\")
      foreach(_m \${_vtkmods})
        execute_process(COMMAND install_name_tool -add_rpath \"@loader_path/../../../../../../Frameworks\" \"\${_m}\" ERROR_QUIET)
      endforeach()
      message(STATUS \"Set Frameworks RPATH on bundled vtkmodules\")
    " COMPONENT Runtime)
  endif()
else()
  # Linux: libvtk*.so come along when package.sh copies the dep lib/ into the
  # package, so they don't need copying here. But the dep-built vtkmodules .so
  # bake in the build-machine's dep lib path as RPATH — repoint them at the
  # package lib/ (one level deeper than shapeworks_py.so in site-packages).
  file(GLOB _vtk_shared_check "${_vtk_lib_dir}/libvtk*.so")
  if(_vtk_shared_check)
    install(CODE "
      set(_install_root \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}\")
      file(GLOB _vtkmods \"\${_install_root}/${_site_packages}/vtkmodules/*.so\")
      find_program(_patchelf patchelf)
      if(_patchelf AND _vtkmods)
        foreach(_m \${_vtkmods})
          # \$ORIGIN: VTK's Linux Python install puts libvtkWrappingPythonCore*.so
          #   (unversioned) inside vtkmodules/, referenced by plain soname beside
          #   the module .so — so the module dir itself must be on the rpath.
          # 6-dotdots+lib: the rest of the shared libvtk* live in the package lib/
          #   (package.sh copies the dep lib/ there). From
          #   lib/python/lib/python3.12/site-packages/vtkmodules that is 6 up + lib.
          execute_process(COMMAND \${_patchelf} --set-rpath \"\$ORIGIN:\$ORIGIN/../../../../../../lib\" \"\${_m}\")
        endforeach()
        message(STATUS \"Set \$ORIGIN + lib/ RPATH on bundled vtkmodules\")
      endif()
    " COMPONENT Runtime)
  endif()
endif()

# ---------------------------------------------------------------------------
# 7. macOS: relocate the shapeworks CLI into the .app so its bundled-Python
#    lookup (../Resources/Python relative to the executable) resolves.
#    Leaves a thin wrapper at install/bin/shapeworks for backward compatibility.
# ---------------------------------------------------------------------------
if(APPLE)
  # Generate wrapper script at configure time using bracket syntax
  # (avoids escape gymnastics inside install(CODE))
  set(_cli_wrapper_dir "${CMAKE_BINARY_DIR}/_bundled_python/cli_wrapper")
  file(MAKE_DIRECTORY "${_cli_wrapper_dir}")
  file(WRITE "${_cli_wrapper_dir}/shapeworks"
[=[#!/bin/bash
LOC="`dirname "$0"`"
"${LOC}/ShapeWorksStudio.app/Contents/MacOS/shapeworks" "$@"
]=])

  # Move the CLI binary into the .app after macdeployqt runs
  install(CODE "
    set(_install_root \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}\")
    set(_cli_src \"\${_install_root}/bin/shapeworks\")
    set(_cli_dest_dir \"\${_install_root}/bin/ShapeWorksStudio.app/Contents/MacOS\")
    set(_cli_dest \"\${_cli_dest_dir}/shapeworks\")
    if(EXISTS \"\${_cli_src}\" AND NOT IS_SYMLINK \"\${_cli_src}\")
      file(MAKE_DIRECTORY \"\${_cli_dest_dir}\")
      file(RENAME \"\${_cli_src}\" \"\${_cli_dest}\")
      message(STATUS \"Relocated shapeworks CLI into .app/Contents/MacOS/\")
    endif()
  " COMPONENT Runtime)

  # Install the wrapper at install/bin/shapeworks (registered after the move so
  # it lands in the now-empty slot rather than overwriting the binary)
  install(PROGRAMS "${_cli_wrapper_dir}/shapeworks" DESTINATION bin)
endif()

# ---------------------------------------------------------------------------
# 8. Install 'swpython' wrapper that launches the bundled Python with
#    PYTHONHOME pointed at the bundled tree. Mirrors ParaView's pvpython:
#      swpython my_script.py             (run a script against bundled python)
#      swpython -m pip install <pkg>     (install into bundled site-packages,
#                                         or pass --target for read-only installs)
# ---------------------------------------------------------------------------
set(_swpython_dir "${CMAKE_BINARY_DIR}/_bundled_python/swpython")
file(MAKE_DIRECTORY "${_swpython_dir}")

if(APPLE)
  file(WRITE "${_swpython_dir}/swpython"
[=[#!/bin/bash
LOC="`dirname "$0"`"
PYTHONHOME="${LOC}/ShapeWorksStudio.app/Contents/Resources/Python"
PATH="${LOC}:${PATH}"
# Default matplotlib to a non-interactive backend. swpython is a CLI tool;
# the macosx backend crashes when invoked from a Background-role process
# (e.g. headless / non-GUI-login session). Users can override by setting
# MPLBACKEND themselves before invoking swpython.
MPLBACKEND="${MPLBACKEND:-Agg}"
unset PYTHONPATH
export PYTHONHOME PATH MPLBACKEND
exec "${PYTHONHOME}/bin/python3" "$@"
]=])
  install(PROGRAMS "${_swpython_dir}/swpython" DESTINATION bin)

elseif(WIN32)
  file(WRITE "${_swpython_dir}/swpython.bat"
[=[@echo off
set "LOC=%~dp0"
set "PYTHONHOME=%LOC%..\lib\python"
set "PATH=%LOC%;%PATH%"
set "PYTHONPATH="
if not defined MPLBACKEND set "MPLBACKEND=Agg"
"%PYTHONHOME%\python.exe" %*
]=])
  install(PROGRAMS "${_swpython_dir}/swpython.bat" DESTINATION bin)

else()
  # Linux — prepend bundle lib/ to LD_LIBRARY_PATH so the dynamic linker
  # finds our libstdc++.so.6 / libgcc_s.so.1 before any pip-installed
  # extension (e.g. VTK wheels) pulls in the system one. The C++ binaries
  # don't need this because their own RPATH loads libstdc++ at exec time,
  # but a pure-Python entry point has nothing loaded yet when the first
  # extension imports.
  # Also prepend the bundle bin/ to PATH so `shutil.which("shapeworks")`
  # in use case scripts finds the CLI sitting next to the wrapper.
  file(WRITE "${_swpython_dir}/swpython"
[=[#!/bin/bash
LOC="`dirname "$0"`"
PYTHONHOME="${LOC}/../lib/python"
LD_LIBRARY_PATH="${LOC}/../lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
PATH="${LOC}:${PATH}"
MPLBACKEND="${MPLBACKEND:-Agg}"
unset PYTHONPATH
export PYTHONHOME LD_LIBRARY_PATH PATH MPLBACKEND
exec "${PYTHONHOME}/bin/python3" "$@"
]=])
  install(PROGRAMS "${_swpython_dir}/swpython" DESTINATION bin)
endif()

# ---------------------------------------------------------------------------
# 9. Install 'swpip' wrapper — pip for the bundled Python that auto-targets
#    the ShapeWorks per-user site-packages on `install`. Thin shell wrapper
#    that delegates to `swpython -m shapeworks.swpip`; the path logic lives
#    in Python so all three platforms share one source of truth.
# ---------------------------------------------------------------------------
set(_swpip_dir "${CMAKE_BINARY_DIR}/_bundled_python/swpip")
file(MAKE_DIRECTORY "${_swpip_dir}")

if(WIN32)
  file(WRITE "${_swpip_dir}/swpip.bat"
[=[@echo off
set "LOC=%~dp0"
call "%LOC%swpython.bat" -m shapeworks.swpip %*
]=])
  install(PROGRAMS "${_swpip_dir}/swpip.bat" DESTINATION bin)
else()
  file(WRITE "${_swpip_dir}/swpip"
[=[#!/bin/bash
LOC="`dirname "$0"`"
exec "${LOC}/swpython" -m shapeworks.swpip "$@"
]=])
  install(PROGRAMS "${_swpip_dir}/swpip" DESTINATION bin)
endif()

message(STATUS "InstallBundledPython: will install bundled Python to ${_app_python_dest}")
