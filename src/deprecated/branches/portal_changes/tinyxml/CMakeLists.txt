FILE(GLOB tinyxml_srcs ./*.cpp)
ADD_LIBRARY (tinyxml STATIC ${tinyxml_srcs})

#INCLUDE_DIRECTORIES(.)
#INSTALL_TARGETS(/lib tinyxml)
#INSTALL_FILES(/include tinyxml.h)
