cmake_minimum_required(VERSION 3.10.2)

project(HelloShapeWorksItk)

set(CMAKE_CXX_STANDARD 17)

# Find ShapeWorks
find_package(ShapeWorks REQUIRED)

SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--disable-new-dtags")

# Find ITK
#find_package(ITK REQUIRED)
#include(${ITK_USE_FILE})

add_executable(HelloShapeWorksItk HelloShapeWorksItk.cpp)

target_link_libraries(HelloShapeWorksItk
  ${ITK_LIBRARIES}
  shapeworks::Optimize
  shapeworks::Groom
  shapeworks::Analyze
  pybind11::embed
  )
