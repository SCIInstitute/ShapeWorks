import setuptools

# Note: our package also needs the compiled portion of the library, which will
# be in ShapeWorks/bin directory when installed, and environment setup functions
# automatically add it.
#
# For development, source ../devenv.sh, passing the root source and build dirs.

setuptools.setup(
   name='shapeworks',
   version='1.0',
   description='Python module for Shapeworks.',
   packages=setuptools.find_packages(),
   install_requires=['requests'], #external packages as dependencies
)
