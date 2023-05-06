configure_file(doxygen.config.in ${ShapeWorks_BINARY_DIR}/Documentation/Doxygen/doxygen.config @ONLY NEWLINE_STYLE UNIX)
configure_file(Module.dox ${ShapeWorks_BINARY_DIR}/Documentation/Doxygen/Module.dox COPYONLY)

# IF you do NOT want the documentation to be generated EVERY time you build the project
# then leave out the 'ALL' keyword from the above command.
add_custom_target(Documentation ALL
  COMMAND ${DOXYGEN_EXECUTABLE} ${ShapeWorks_BINARY_DIR}/Documentation/Doxygen/doxygen.config
  MAIN_DEPENDENCY ${ShapeWorks_BINARY_DIR}/Documentation/Doxygen/doxygen.config
  WORKING_DIRECTORY ${ShapeWorks_BINARY_DIR}/Documentation/Doxygen
)
