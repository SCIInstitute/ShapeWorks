 
PROJECT(stl2ply)
cmake_minimum_required(VERSION 2.8)
ADD_EXECUTABLE(stl2ply stl2ply.cxx ${source_files})
TARGET_LINK_LIBRARIES(stl2ply ${VTK_LIBRARIES} )
INSTALL(TARGETS stl2ply  RUNTIME DESTINATION .)
