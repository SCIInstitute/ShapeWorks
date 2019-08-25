if(POLICY CMP0020)
    cmake_policy(SET CMP0020 NEW)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

FILE(GLOB SRCS "*.h" "*.cc" ".hxx")

FILE(GLOB HDRS */*.h)

ADD_LIBRARY(Surfworks STATIC ${SRCS} ${HDRS})
TARGET_LINK_LIBRARIES(Surfworks ShapeWorksGroom ${ITK_LIBRARIES} ${VTK_LIBRARIES} PreViewMeshQC)
