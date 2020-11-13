import setuptools

setuptools.setup(
   name='DocumentationUtils',
   version='2.0',
   description='Python functions for ShapeWorks auto-documentation',
   packages=setuptools.find_packages(),
   install_requires=['requests'], #external packages as dependencies
)
