IF(BUILD_32BIT_PARAMS)
    MESSAGE( STATUS "params library: building for 32 bit architecture" )
    ADD_LIBRARY(params param.cxx y.tab.cxx lex.yy.cxx)
ENDIF(BUILD_32BIT_PARAMS)

IF(BUILD_64BIT_PARAMS AND CMAKE_SIZEOF_VOID_P EQUAL 8)
    MESSAGE( STATUS "params library: building for 64 bit architecture" )
    ADD_LIBRARY(params param.cxx y.tab.64.cxx lex.yy.64.cxx)
ENDIF(BUILD_64BIT_PARAMS AND CMAKE_SIZEOF_VOID_P EQUAL 8 )

IF(BUILD_64BIT_PARAMS AND CMAKE_SIZEOF_VOID_P EQUAL 4)
    MESSAGE( FATAL_ERROR "params library: you have selected a 64 bit
    architecture, but CMAKE detects a 32-bit architecture" )
ENDIF(BUILD_64BIT_PARAMS AND CMAKE_SIZEOF_VOID_P EQUAL 4 )

#INCLUDE_DIRECTORIES(.)
#INSTALL_TARGETS(/lib params)
#INSTALL_FILES(/include param.h)
