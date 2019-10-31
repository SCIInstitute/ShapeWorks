import setuptools

setuptools.setup(
   name='GirderConnector',
   version='1.0',
   description='Python functions for calling the Girder API',
   author='asdf',
   author_email='asdf@asdf.com',
   packages=setuptools.find_packages(),
   install_requires=['requests'], #external packages as dependencies
)