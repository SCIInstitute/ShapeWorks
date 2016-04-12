include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/Application/TinyXML)
include_directories(${CMAKE_SOURCE_DIR}/Application)

FILE(GLOB SRCS "*.h" "*.cxx")

ADD_LIBRARY(ShapeWorksGroom ${SRCS})
