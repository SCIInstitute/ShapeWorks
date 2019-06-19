 
project(RemoveFidsDTLeakage)
cmake_minimum_required(VERSION 2.8)
ADD_EXECUTABLE(RemoveFidsDTLeakage RemoveFidsDTLeakage.cxx ${source_files})
TARGET_LINK_LIBRARIES(RemoveFidsDTLeakage ${ITK_LIBRARIES} tinyxml)
INSTALL(TARGETS RemoveFidsDTLeakage RUNTIME DESTINATION .)



