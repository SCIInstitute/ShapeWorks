import setuptools

# Note: our package also needs the compiled portion of the library, which will
# be in ShapeWorks/bin directory when installed, and environment setup functions
# automatically add it. For development, just ensure PYTHONPATH begins with
# `/path/to/code/ShapeWorks/Python/shapeworks` and it should all work.

setuptools.setup(
   name='shapeworks',
   version='1.0',
   description='Python module for Shapeworks.',
   packages=setuptools.find_packages(),
   install_requires=['requests'], #external packages as dependencies
)
