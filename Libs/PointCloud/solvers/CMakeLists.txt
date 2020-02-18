project(PointCloudSolvers CXX)

set(SOURCE_FILES "")
set(HEADER_FILES "")

include_directories(.)
include_directories(../)

list (APPEND SOURCE_FILES EigenRBFSolver.cpp)
list (APPEND HEADER_FILES EigenRBFSolver.h)

add_library(PointCloudSolvers ${SOURCE_FILES} ${HEADER_FILES})

target_link_libraries (PointCloudSolvers PUBLIC
    Eigen3::Eigen)
