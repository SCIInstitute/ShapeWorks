include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/Application)

FILE(GLOB SRCS "*.txx" "*.h" "*.cpp")

ADD_LIBRARY(ShapeWorksGroom STATIC ${SRCS})
TARGET_LINK_LIBRARIES(ShapeWorksGroom ${ITK_LIBRARIES} tinyxml)
