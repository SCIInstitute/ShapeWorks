import setuptools
import shutil
import glob
import os 

filename = glob.glob("../../build/bin/shapeworks.cpython*")

shutil.copyfile(filename,os.getcwd())
setuptools.setup(
   name='shapeworks',
   version='1.0',
   description='Python module for Shapeworks.',
   packages=setuptools.find_packages(),
   install_requires=['requests'], #external packages as dependencies
)