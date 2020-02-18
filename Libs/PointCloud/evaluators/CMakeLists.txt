PROJECT(PointCloudEvaluators CXX)

include_directories(.)
include_directories(../)

set(CPU_SOURCE_FILES
  CPURBFEvaluator.cpp)

set(CPU_HEADER_FILES
  RBFEvaluator.h
  CPURBFEvaluator.h)

add_library(PointCloudEvaluators ${CPU_SOURCE_FILES} ${CPU_HEADER_FILES})

target_link_libraries (PointCloudEvaluators PUBLIC
    Eigen3::Eigen)
