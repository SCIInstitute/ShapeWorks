 
project(GetFeatureVolume)
cmake_minimum_required(VERSION 2.8)
ADD_EXECUTABLE(GetFeatureVolume GetFeatureVolume.cxx)
TARGET_LINK_LIBRARIES(GetFeatureVolume ${ITK_LIBRARIES} ${VTK_LIBRARIES} ${VXL_LIBRARIES} vgl_algo vgl fim_v4 tinyxml)
INSTALL(TARGETS GetFeatureVolume RUNTIME DESTINATION .)