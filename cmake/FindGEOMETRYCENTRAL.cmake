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
  
  # Check if geometry-central was built with SuiteSparse support
  # by looking for CHOLMOD symbols in the static library
  if(UNIX AND NOT APPLE)
    execute_process(
      COMMAND nm "${GEOMETRYCENTRAL_LIBRARY}"
      COMMAND grep -q "cholmod"
      RESULT_VARIABLE CHOLMOD_CHECK
      OUTPUT_QUIET ERROR_QUIET
    )
    
    if(CHOLMOD_CHECK EQUAL 0)
      message(STATUS "Geometry-central appears to use SuiteSparse, adding dependencies")
      
      # Find SuiteSparse libraries
      find_library(CHOLMOD_LIB cholmod)
      find_library(SUITESPARSE_CONFIG_LIB suitesparseconfig)
      find_library(AMD_LIB amd)
      find_library(COLAMD_LIB colamd)
      find_library(CAMD_LIB camd)
      find_library(CCOLAMD_LIB ccolamd)
      find_library(METIS_LIB metis)
      
      # Find BLAS and LAPACK
      find_package(BLAS)
      find_package(LAPACK)
      
      # Build the complete library list
      set(SUITESPARSE_DEPS "")
      
      if(CHOLMOD_LIB)
        list(APPEND SUITESPARSE_DEPS ${CHOLMOD_LIB})
      endif()
      if(SUITESPARSE_CONFIG_LIB)
        list(APPEND SUITESPARSE_DEPS ${SUITESPARSE_CONFIG_LIB})
      endif()
      if(AMD_LIB)
        list(APPEND SUITESPARSE_DEPS ${AMD_LIB})
      endif()
      if(COLAMD_LIB)
        list(APPEND SUITESPARSE_DEPS ${COLAMD_LIB})
      endif()
      if(CAMD_LIB)
        list(APPEND SUITESPARSE_DEPS ${CAMD_LIB})
      endif()
      if(CCOLAMD_LIB)
        list(APPEND SUITESPARSE_DEPS ${CCOLAMD_LIB})
      endif()
      if(METIS_LIB)
        list(APPEND SUITESPARSE_DEPS ${METIS_LIB})
      endif()
      if(BLAS_FOUND)
        list(APPEND SUITESPARSE_DEPS ${BLAS_LIBRARIES})
      endif()
      if(LAPACK_FOUND)
        list(APPEND SUITESPARSE_DEPS ${LAPACK_LIBRARIES})
      endif()
      
      # Append dependencies to GEOMETRYCENTRAL_LIBRARIES
      list(APPEND GEOMETRYCENTRAL_LIBRARIES ${SUITESPARSE_DEPS})
      
      message(STATUS "Added SuiteSparse dependencies: ${SUITESPARSE_DEPS}")
    endif()
  endif()
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
  
  # Add interface link libraries for proper dependency propagation
  if(SUITESPARSE_DEPS)
    set_target_properties(geometrycentral PROPERTIES
      INTERFACE_LINK_LIBRARIES "${SUITESPARSE_DEPS}")
  endif()
endif()
