import setuptools

setuptools.setup(
   name='shapeworks',
   version='1.0',
   description='Python module for Shapeworks.',
   packages=setuptools.find_packages(),
   install_requires=['requests'], #external packages as dependencies
   package_data={
   				
   					'shapeworks':
   					['shapeworks/shapeworks.cpython-37m-x86_64-linux-gnu.so',
   					]
   					
   				},
)