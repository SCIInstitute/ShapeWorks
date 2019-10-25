project( FIM_V4 )
cmake_minimum_required(VERSION 2.5)

# Match everything when analyzing dependencies
INCLUDE_REGULAR_EXPRESSION("^.*$")
#add_definitions( -Wall )

#todo: these two lines are issues since OPTION is deprecated:
option (BUILD_SHARED_LIBS "Build shared libraries" OFF)
option (INSTALL_SOURCE  "Install FIM_V4 source code." OFF)

# Build the trimesh2 library with the meshFIM code included
file(GLOB trimesh2_srcs trimesh2/include/*.h trimesh2/libsrc/*.cc)
set( FIM_SRCS meshFIM.cpp ${trimesh2_srcs} )

add_library(fim_v4 STATIC ${FIM_SRCS})
target_include_directories(fim_v4 PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<INSTALL_INTERFACE:include>)
target_include_directories(fim_v4 PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/trimesh2/include> $<INSTALL_INTERFACE:trimesh/include>)  #ctc: move trimesh2 to main external libs (see github issue #89)

# INSTALLATION AND PACKAGING

# # Install the headers
# file (GLOB trimesh_headers ${CMAKE_SOURCE_DIR}/trimesh2/include/*.h)
# INSTALL(FILES ${trimesh_headers} meshFIM.h DESTINATION include)

#ctc set public_header properties

# Install the library
INSTALL(TARGETS fim_v4
 RUNTIME DESTINATION bin  #p[robably lib since this is a library, no executables
 LIBRARY DESTINATION lib
 ARCHIVE DESTINATION lib
 PUBLIC_HEADER DESTINATION include
)

# # To Create a package, one can run "cpack -G DragNDrop CPackConfig.cmake" on Mac OS X
# # where CPackConfig.cmake is created by including CPack
# # And then there's ways to customize this as well
# set(CPACK_BINARY_DRAGNDROP ON)
# set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "FIM Research Sandbox")
# set(CPACK_PACKAGE_VENDOR "University of Utah")
# set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/PackageDescription.txt")
# set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/FIMLicense.txt")
# set(CPACK_PACKAGE_VERSION_MAJOR "0")
# set(CPACK_PACKAGE_VERSION_MINOR "1")
# set(CPACK_PACKAGE_VERSION_PATCH "0")
# set(CPACK_PACKAGE_INSTALL_DIRECTORY "FIM ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

# include(CPack)
