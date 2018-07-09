PROJECT(shapeworks)
cmake_minimum_required(VERSION 2.6)
SET(CMAKE_BUILD_TYPE RELEASE)

SET(ITK_DIR "/Users/prafulag/Installations/Build/ITK-4.7.2")
SET(VTK_DIR "/Users/prafulag/Installations/Build/VTK-5.10.1")
SET(VXL_DIR "/Users/prafulag/Installations/Build/vxl")

include_directories (
${SHAPEWORKS_SOURCE_DIR}/ShapeWorks-Prep/source
${SHAPEWORKS_SOURCE_DIR}/ShapeWorks-Run/source
${SHAPEWORKS_SOURCE_DIR}/ShapeWorks-Post/source/ShapeWorksPost-V1
${SHAPEWORKS_SOURCE_DIR}/ShapeWorks-Post/source/ShapeWorksView2
)

subdirs(ShapeWorks-Prep/source ShapeWorks-Run/source ShapeWorks-Post/source/ShapeWorksPost-V1 ShapeWorks-Post/source/ShapeWorksView2)
