To build and link with ShapeWorks:

* Configure with CMake
* Make sure to specify the CMAKE_INSTALL_PREFIX variable if you don't want it to install in a standard system location.
* Run "make install"

* Select "BUILD_Mesh_Support" when configuring ShapeWorks in CMake
* If you have installed fim in a reasonable location, then the ShapeWorks config will automatically find it.  Otherwise, you'll need to specify the FIM_LIB and FIM_INCLUDE_PATH variables.
