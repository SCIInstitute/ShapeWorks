include_directories(${CMAKE_CURRENT_SOURCE_DIR})

IF (WIN32 AND MSVC)
  ADD_DEFINITIONS(-D_SCL_SECURE_NO_WARNINGS)
  ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
ENDIF (WIN32 AND MSVC)

#--------------------------------------------------------------------------------
### Projects
#--------------------------------------------------------------------------------

add_subdirectory(Groom)
add_subdirectory(Application)

