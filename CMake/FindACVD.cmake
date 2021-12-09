set(ACVD_DIR "$ENV{ACVD_DIR}" CACHE PATH "ACVD root directory.")
message(STATUS "Looking for ACVD in ${ACVD_DIR}")

find_path(ACVD_INCLUDE_DIR
  NAMES ACVD/DiscreteRemeshing/vtkIsotropicDiscreteRemeshing.h
  HINTS ${ACVD_DIR}/include
  )

find_library(ACVD_DISCRETE_LIBRARY 
  libvtkDiscreteRemeshing
  NAMES libvtkDiscreteRemeshing.dylib libvtkDiscreteRemeshing.so vtkDiscreteRemeshing.lib
  HINTS /usr/local/lib
  HINTS ${ACVD_DIR}/build/source
  HINTS ${ACVD_DIR}/lib
  )
find_library(ACVD_VOLUME_LIBRARY 
  libvtkVolumeProcessing
  NAMES libvtkVolumeProcessing.dylib libvtkVolumeProcessing.so vtkVolumeProcessing.lib
  HINTS /usr/local/lib
  HINTS ${ACVD_DIR}/build/source
  HINTS ${ACVD_DIR}/lib
  )
find_library(ACVD_SURFACE_LIBRARY 
  libvtkSurface
  NAMES libvtkSurface.dylib libvtkSurface.so vtkSurface.lib
  HINTS /usr/local/lib
  HINTS ${ACVD_DIR}/build/source
  HINTS ${ACVD_DIR}/lib
  )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ACVD DEFAULT_MSG ACVD_DISCRETE_LIBRARY ACVD_INCLUDE_DIR)

if(ACVD_FOUND)
  message("-- Found ACVD under ${ACVD_INCLUDE_DIR}")
  set(ACVD_INCLUDE_DIRS
    "${ACVD_INCLUDE_DIR}/ACVD/DiscreteRemeshing"
    "${ACVD_INCLUDE_DIR}/ACVD/Common"
    "${ACVD_INCLUDE_DIR}/ACVD/VolumeProcessing"
    )

  set(ACVD_LIBRARIES
    ${ACVD_DISCRETE_LIBRARY}
    ${ACVD_VOLUME_LIBRARY}
    ${ACVD_SURFACE_LIBRARY}
    )

endif(ACVD_FOUND)

mark_as_advanced(ACVD_LIBRARY ACVD_INCLUDE_DIR)

if(ACVD_FOUND AND NOT (TARGET acvd::acvd))
  # debug output showing the located libraries
  message(STATUS "ACVD_INCLUDE_DIRS=${ACVD_INCLUDE_DIRS}")
  message(STATUS "ACVD_LIBRARIES=${ACVD_LIBRARIES}")

  add_library(acvd::discrete UNKNOWN IMPORTED)
  set_property(TARGET acvd::discrete APPEND PROPERTY
    IMPORTED_LOCATION "${ACVD_DISCRETE_LIBRARY}")
  # add the transitive includes property
  set_target_properties(acvd::discrete PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${ACVD_INCLUDE_DIRS}")

  add_library(acvd::surface UNKNOWN IMPORTED)
  set_property(TARGET acvd::surface APPEND PROPERTY
    IMPORTED_LOCATION "${ACVD_SURFACE_LIBRARY}")
  # add the transitive includes property
  set_target_properties(acvd::surface PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${ACVD_INCLUDE_DIRS}")

  add_library(acvd::volume UNKNOWN IMPORTED)
  set_property(TARGET acvd::volume APPEND PROPERTY
    IMPORTED_LOCATION "${ACVD_VOLUME_LIBRARY}")
  # add the transitive includes property
  set_target_properties(acvd::volume PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${ACVD_INCLUDE_DIRS}")
  
  # Add a blank imported library
  add_library(acvd::acvd INTERFACE IMPORTED)
  set_target_properties(acvd::acvd PROPERTIES
    INTERFACE_LINK_LIBRARIES "acvd::discrete;acvd::surface;acvd::volume"
    )
endif()
