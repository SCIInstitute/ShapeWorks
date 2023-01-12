
set(GEOMETRYCENTRAL_DIR "$ENV{GEOMETRYCENTRAL_DIR}" CACHE PATH "GEOMETRYCENTRAL root directory.")
message(STATUS "Looking for GEOMETRYCENTRAL in ${GEOMETRYCENTRAL_DIR}")

find_path(GEOMETRYCENTRAL_INCLUDE_DIR
  NAMES geometrycentral/surface/surface_mesh.h
  HINTS /usr/local/include
  HINTS ${GEOMETRYCENTRAL_DIR}/include
  )

if(APPLE)
  find_library(GEOMETRYCENTRAL_LIBRARY 
    libgeometry-central.a
    HINTS /usr/local/lib
    HINTS ${GEOMETRYCENTRAL_DIR}/lib
    )
elseif(UNIX)
  find_library(GEOMETRYCENTRAL_LIBRARY 
    libgeometry-central.a
    HINTS /usr/local/lib
    HINTS ${GEOMETRYCENTRAL_DIR}/lib
    )
elseif(WIN32)
  find_library(GEOMETRYCENTRAL_LIBRARY 
    geometry-central.lib
    HINTS ${GEOMETRYCENTRAL_DIR}/lib
    )
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GEOMETRYCENTRAL DEFAULT_MSG GEOMETRYCENTRAL_LIBRARY GEOMETRYCENTRAL_INCLUDE_DIR)

if(GEOMETRYCENTRAL_FOUND)
  message("-- Found GEOMETRYCENTRAL under ${GEOMETRYCENTRAL_INCLUDE_DIR}")
  set(GEOMETRYCENTRAL_INCLUDE_DIRS ${GEOMETRYCENTRAL_INCLUDE_DIR})
  set(GEOMETRYCENTRAL_LIBRARIES ${GEOMETRYCENTRAL_LIBRARY})
endif(GEOMETRYCENTRAL_FOUND)

mark_as_advanced(GEOMETRYCENTRAL_LIBRARY GEOMETRYCENTRAL_INCLUDE_DIR)

if(GEOMETRYCENTRAL_FOUND AND NOT (TARGET geometrycentral))
  # debug output showing the located libraries
  message(STATUS "GEOMETRYCENTRAL_INCLUDE_DIR=${GEOMETRYCENTRAL_INCLUDE_DIR}")
  message(STATUS "GEOMETRYCENTRAL_LIBRARY=${GEOMETRYCENTRAL_LIBRARY}")
  message(STATUS "GEOMETRYCENTRAL_LIBRARIES=${GEOMETRYCENTRAL_LIBRARIES}")
  # Add a blank imported library
  add_library(geometrycentral UNKNOWN IMPORTED)
  set_property(TARGET geometrycentral APPEND PROPERTY
    IMPORTED_LOCATION "${GEOMETRYCENTRAL_LIBRARY}")
  # add the transitive includes property
  set_target_properties(geometrycentral PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${GEOMETRYCENTRAL_INCLUDE_DIR}")
endif()
