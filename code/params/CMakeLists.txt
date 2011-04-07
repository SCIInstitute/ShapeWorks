IF( CMAKE_SIZEOF_VOID_P EQUAL 4 )
    MESSAGE( STATUS "32 bit architecture detected" )
    ADD_LIBRARY(params param.cxx y.tab.cxx lex.yy.cxx)
ENDIF(CMAKE_SIZEOF_VOID_P EQUAL 4)

IF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    MESSAGE( STATUS "64 bit architecture detected" )
    ADD_LIBRARY(params param.cxx y.tab.64.cxx lex.yy.64.cxx)
ENDIF(CMAKE_SIZEOF_VOID_P EQUAL 8)

#INCLUDE_DIRECTORIES(.)
#INSTALL_TARGETS(/lib params)
#INSTALL_FILES(/include param.h)
