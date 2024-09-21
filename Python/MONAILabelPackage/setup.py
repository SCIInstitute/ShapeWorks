import setuptools

setuptools.setup(
   name='MONAILabel',
   version='1.0',
   description='MONAI client interface',
   packages=setuptools.find_packages(),
   install_requires=['requests'],
)