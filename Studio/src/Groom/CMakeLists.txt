include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/TinyXML)
include_directories(${CMAKE_SOURCE_DIR}/Application)

FILE(GLOB SRCS "*.txx" "*.h" "*.cxx")

ADD_LIBRARY(ShapeWorksGroom ${SRCS})
TARGET_LINK_LIBRARIES(ShapeWorksGroom ${ITK_LIBRARIES})
