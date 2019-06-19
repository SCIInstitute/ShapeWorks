cmake_minimum_required(VERSION 2.8.12)
project(libigl)

### Available options ###
option(LIBIGL_USE_STATIC_LIBRARY    "Use libigl as static library" OFF)
option(LIBIGL_WITH_CGAL             "Use CGAL"           OFF)
option(LIBIGL_WITH_NANOGUI          "Use Nanogui menu"   OFF)
option(LIBIGL_WITH_OPENGL           "Use OpenGL"         OFF)
option(LIBIGL_WITH_TETGEN           "Use Tetgen"         OFF)
option(LIBIGL_WITH_VIEWER           "Use OpenGL viewer"  ON)



### Compilation configuration ###
if(MSVC)
  ### Enable parallel compilation for Visual Studio
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /bigobj")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /w") # disable all warnings (not ideal but...)
else()
  #### Libigl requires a modern C++ compiler that supports c++11
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w") # disable all warnings (not ideal but...)
endif()
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

### OpenMP ### (OpenMP is disable for now)
#find_package(OpenMP)
#if(OPENMP_FOUND AND NOT WIN32)
#  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#  list(APPEND LIBIGL_DEFINITIONS "${OpenMP_CXX_FLAGS}")
#endif()


### Compiling libraries based on chosen options ###
set(LIBIGL_INCLUDE_DIRS "")
set(LIBIGL_LIBRARIES "")
set(LIBIGL_EXTRA_LIBRARIES "")
set(LIBIGL_DEFINITIONS "")

if(LIBIGL_USE_STATIC_LIBRARY)
  list(APPEND LIBIGL_DEFINITIONS "-DIGL_STATIC_LIBRARY")
endif()

### macro definition ###
set(LIBIGL_ROOT "${PROJECT_SOURCE_DIR}/../..")
set(LIBIGL_SOURCE_DIR "${LIBIGL_ROOT}/include")
set(LIBIGL_EXTERNAL "${LIBIGL_ROOT}/external")

### Eigen ###
set(EIGEN_INCLUDE_DIR "${LIBIGL_EXTERNAL}/nanogui/ext/eigen")
list(APPEND LIBIGL_INCLUDE_DIRS "${EIGEN_INCLUDE_DIR}")

macro(CompileIGL_Module module)
  file(GLOB SOURCES_IGL_${module}
  "${LIBIGL_SOURCE_DIR}/igl/${module}/*.cpp"
  )
  add_library(igl${module} STATIC ${SOURCES_IGL_${module}})
  target_include_directories(igl${module} PRIVATE ${EIGEN_INCLUDE_DIR})
  target_include_directories(igl${module} PRIVATE ${LIBIGL_SOURCE_DIR})
  target_compile_definitions(igl${module} PRIVATE -DIGL_STATIC_LIBRARY)
  list(APPEND LIBIGL_LIBRARIES "igl${module}")
endmacro()

macro(CompileIGL_Module_Copyleft module)
  file(GLOB SOURCES_IGL_${module}
  "${LIBIGL_SOURCE_DIR}/igl/copyleft/${module}/*.cpp"
  )
  add_library(igl${module} STATIC ${SOURCES_IGL_${module}})
  target_include_directories(igl${module} PRIVATE ${EIGEN_INCLUDE_DIR})
  target_include_directories(igl${module} PRIVATE ${LIBIGL_SOURCE_DIR})
  target_compile_definitions(igl${module} PRIVATE -DIGL_STATIC_LIBRARY)
  list(APPEND LIBIGL_LIBRARIES "igl${module}")
endmacro()


### IGL Common ###
list(APPEND LIBIGL_INCLUDE_DIRS "${LIBIGL_SOURCE_DIR}")
if(LIBIGL_USE_STATIC_LIBRARY)
  file(GLOB SOURCES_IGL
  "${LIBIGL_SOURCE_DIR}/igl/*.cpp"
  "${LIBIGL_SOURCE_DIR}/igl/copyleft/*.cpp")
  add_library(igl STATIC ${SOURCES_IGL})
  target_include_directories(igl PRIVATE ${EIGEN_INCLUDE_DIR})
  target_include_directories(igl PRIVATE ${LIBIGL_SOURCE_DIR})
  target_compile_definitions(igl PRIVATE -DIGL_STATIC_LIBRARY)
  list(APPEND LIBIGL_LIBRARIES "igl")
endif()


### Compile the cgal parts ###
if(LIBIGL_WITH_CGAL) # to be cleaned
  find_package(CGAL REQUIRED)
  # set(Boost_USE_MULTITHREADED      ON)
  # set(Boost_USE_STATIC_LIBS      ON)
  #
  # find_package(BOOST REQUIRED)
  include(${CGAL_USE_FILE})
  list(APPEND LIBIGL_INCLUDE_DIRS ${CGAL_3RD_PARTY_INCLUDE_DIRS})
  list(APPEND LIBIGL_INCLUDE_DIRS ${CGAL_INCLUDE_DIRS})
  list(APPEND LIBIGL_CGAL_EXTRA_LIBRARIES ${CGAL_3RD_PARTY_LIBRARIES})
  list(APPEND LIBIGL_CGAL_EXTRA_LIBRARIES ${CGAL_LIBRARIES})
  list(APPEND LIBIGL_EXTRA_LIBRARIES ${LIBIGL_CGAL_EXTRA_LIBRARIES})
  if(LIBIGL_USE_STATIC_LIBRARY)
    CompileIGL_Module_Copyleft("cgal")
    target_include_directories(iglcgal PRIVATE
      ${CGAL_3RD_PARTY_INCLUDE_DIRS}
      ${CGAL_INCLUDE_DIRS})
  endif()
endif()


### Compile the opengl parts ###
if(LIBIGL_WITH_OPENGL)
  if(LIBIGL_USE_STATIC_LIBRARY)
    CompileIGL_Module("opengl")
    CompileIGL_Module("opengl2")
    if(NOT APPLE)
      target_include_directories(iglopengl PRIVATE "${LIBIGL_EXTERNAL}/nanogui/ext/glew/include")
      target_include_directories(iglopengl2 PRIVATE "${LIBIGL_EXTERNAL}/nanogui/ext/glew/include")
    endif()
  endif()
endif()

### Compile the tetgen part ###
if(LIBIGL_WITH_TETGEN)
  set(TETGEN_DIR "${LIBIGL_EXTERNAL}/tetgen")
  add_subdirectory("${TETGEN_DIR}" "tetgen")
  list(APPEND LIBIGL_INCLUDE_DIRS ${TETGEN_DIR})
  list(APPEND LIBIGL_TETGEN_EXTRA_LIBRARIES "tetgen")
  list(APPEND LIBIGL_EXTRA_LIBRARIES ${LIBIGL_TETGEN_EXTRA_LIBRARIES})

  if(LIBIGL_USE_STATIC_LIBRARY)
    CompileIGL_Module_Copyleft("tetgen")
    target_include_directories(igltetgen PRIVATE ${TETGEN_DIR})
  endif()
endif()

### Compile the viewer ###
if(LIBIGL_WITH_VIEWER)
  set(NANOGUI_DIR "${LIBIGL_EXTERNAL}/nanogui")

  if(LIBIGL_WITH_NANOGUI)
    list(APPEND LIBIGL_DEFINITIONS "-DIGL_VIEWER_WITH_NANOGUI")

    if (LIBIGL_WITH_PYTHON)
      set(NANOGUI_BUILD_PYTHON ON CACHE BOOL " " FORCE)
    else()
      set(NANOGUI_BUILD_PYTHON OFF CACHE BOOL " " FORCE)
    endif()

    set(NANOGUI_BUILD_EXAMPLE OFF CACHE BOOL " " FORCE)
    set(NANOGUI_BUILD_SHARED  OFF CACHE BOOL " " FORCE)
    add_subdirectory("${NANOGUI_DIR}" "nanogui")

    set(VIEWER_INCLUDE_DIRS
      "${NANOGUI_DIR}/include"
      "${NANOGUI_DIR}/ext/nanovg/src"
      "${NANOGUI_DIR}/ext/glfw/include")
    set(LIBIGL_VIEWER_EXTRA_LIBRARIES "nanogui" ${NANOGUI_EXTRA_LIBS})
  else()
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL " " FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL " " FORCE)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL " " FORCE)
    set(GLFW_BUILD_INSTALL OFF CACHE BOOL " " FORCE)
    add_subdirectory("${NANOGUI_DIR}/ext/glfw" "glfw")

    set(VIEWER_INCLUDE_DIRS "${NANOGUI_DIR}/ext/glfw/include")
    set(LIBIGL_VIEWER_EXTRA_LIBRARIES "glfw" ${GLFW_LIBRARIES})
  endif()

  ### GLEW for linux and windows
  if((UNIX AND NOT APPLE) OR WIN32) ### Compile glew if needed
    set(GLEW_INSTALL OFF CACHE BOOL " " FORCE)
    add_subdirectory("${NANOGUI_DIR}/ext/glew" "glew")
  endif()
  if(NOT APPLE)
    list(APPEND VIEWER_INCLUDE_DIRS "${NANOGUI_DIR}/ext/glew/include")
    list(APPEND LIBIGL_VIEWER_EXTRA_LIBRARIES "glew")
  endif()

  list(APPEND LIBIGL_INCLUDE_DIRS ${VIEWER_INCLUDE_DIRS})
  list(APPEND LIBIGL_EXTRA_LIBRARIES ${LIBIGL_VIEWER_EXTRA_LIBRARIES})

  if(LIBIGL_USE_STATIC_LIBRARY)
    CompileIGL_Module("viewer")
    if(LIBIGL_WITH_NANOGUI)
      target_compile_definitions(iglviewer PRIVATE -DIGL_VIEWER_WITH_NANOGUI)
    endif()
    target_include_directories(iglviewer PRIVATE ${VIEWER_INCLUDE_DIRS})
  endif()
endif()


# Function to print list nicely
function(print_list title list)
  message("-- ${title}:")
  foreach(elt ${list})
    message("\t ${elt}")
  endforeach()
endfunction()

# Pass the list of compiled libraries to the parent if there is one
if(NOT ${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
  list(REVERSE LIBIGL_LIBRARIES)

  set(LIBIGL_INCLUDE_DIRS ${LIBIGL_INCLUDE_DIRS} PARENT_SCOPE)
  set(LIBIGL_LIBRARIES ${LIBIGL_LIBRARIES} PARENT_SCOPE)
  set(LIBIGL_ANTTWEAKBAR_EXTRA_LIBRARIES ${LIBIGL_ANTTWEAKBAR_EXTRA_LIBRARIES} PARENT_SCOPE)
  set(LIBIGL_CGAL_EXTRA_LIBRARIES        ${LIBIGL_CGAL_EXTRA_LIBRARIES}        PARENT_SCOPE)
  set(LIBIGL_NANOGUI_EXTRA_LIBRARIES     ${LIBIGL_NANOGUI_EXTRA_LIBRARIES}     PARENT_SCOPE)
  set(LIBIGL_OPENGL_EXTRA_LIBRARIES      ${LIBIGL_OPENGL_EXTRA_LIBRARIES}      PARENT_SCOPE)
  set(LIBIGL_TETGEN_EXTRA_LIBRARIES      ${LIBIGL_TETGEN_EXTRA_LIBRARIES}      PARENT_SCOPE)
  set(LIBIGL_TRIANGLE_EXTRA_LIBRARIES    ${LIBIGL_TRIANGLE_EXTRA_LIBRARIES}    PARENT_SCOPE)
  set(LIBIGL_VIEWER_EXTRA_LIBRARIES      ${LIBIGL_VIEWER_EXTRA_LIBRARIES}      PARENT_SCOPE)
  set(LIBIGL_XML_EXTRA_LIBRARIES         ${LIBIGL_XML_EXTRA_LIBRARIES}         PARENT_SCOPE)
  set(LIBIGL_EXTRA_LIBRARIES ${LIBIGL_EXTRA_LIBRARIES} PARENT_SCOPE)
  set(LIBIGL_DEFINITIONS ${LIBIGL_DEFINITIONS} PARENT_SCOPE)

  ### ligIGL information ###
  print_list("libigl includes" "${LIBIGL_INCLUDE_DIRS}")
  print_list("libigl libraries" "${LIBIGL_LIBRARIES}")
  print_list("libigl extra libraries" "${LIBIGL_EXTRA_LIBRARIES}")
  print_list("libigl definitions" "${LIBIGL_DEFINITIONS}")
endif()
