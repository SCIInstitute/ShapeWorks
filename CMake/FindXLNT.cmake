set(XLNT_DIR "$ENV{XLNT_DIR}" CACHE PATH "XLNT root directory.")
message("Looking for XLNT in ${XLNT_DIR}")


find_path(XLNT_INCLUDE_DIR
	NAMES xlnt/xlnt.hpp 
	HINTS /usr/local/include
	HINTS ext_lib/xlnt-master/include/xlnt
	HINTS ${XLNT_DIR}/include
)

if(APPLE)
find_library(XLNT_LIBRARY 
	libxlnt.a
	HINTS /usr/local/lib
	HINTS ext_lib/xlnt-master/build/source
	HINTS ${XLNT_DIR}/lib
)
elseif(UNIX)
find_library(XLNT_LIBRARY 
	libxlnt.a 
	HINTS /usr/local/lib
	HINTS ext_lib/xlnt-master/build/source
	HINTS ${XLNT_DIR}/build/source
	HINTS ${XLNT_DIR}/lib
)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(XLNT DEFAULT_MSG XLNT_LIBRARY XLNT_INCLUDE_DIR)

if(XLNT_FOUND)
  message("—- Found XLNT under ${XLNT_INCLUDE_DIR}")
  set(XLNT_INCLUDE_DIRS ${XLNT_INCLUDE_DIR})
  set(XLNT_LIBRARIES ${XLNT_LIBRARY})
  if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(XLNT_LIBRARIES "${XLNT_LIBRARIES};m;pthread")
  endif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
endif(XLNT_FOUND)

mark_as_advanced(XLNT_LIBRARY XLNT_INCLUDE_DIR)

if(XLNT_FOUND AND NOT (TARGET xlnt::xlnt))
  # debug output showing the located libraries
  message(STATUS "XLNT_INCLUDE_DIR=${XLNT_INCLUDE_DIR}")
  message(STATUS "XLNT_LIBRARY=${XLNT_LIBRARY}")
  message(STATUS "XLNT_LIBRARIES=${XLNT_LIBRARIES}")
  # Add a blank imported library
  add_library(xlnt::xlnt UNKNOWN IMPORTED)
  set_property(TARGET xlnt::xlnt APPEND PROPERTY
    IMPORTED_LOCATION "${XLNT_LIBRARY}")
  # add the transitive includes property
  set_target_properties(xlnt::xlnt PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${XLNT_INCLUDE_DIR}")
endif()
