import os
import sys
import numpy as np
from shapeworks import *

success = True

def resampleTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.resample([1, 1, 1])

  compareImg = Image(os.environ["DATA"] + "/resample1.nrrd")

  return img.compare(compareImg)

success &= utils.test(resampleTest1)

def resampleTest2():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.resample(1.5)

  compareImg = Image(os.environ["DATA"] + "/resample2.nrrd")

  return img.compare(compareImg)

success &= utils.test(resampleTest2)

def resampleTest3():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.resample([1.5, 1.5, 1.5])

  compareImg = Image(os.environ["DATA"] + "/resample3.nrrd")

  return img.compare(compareImg)

success &= utils.test(resampleTest3)

def resampleTest4():
  img = Image(os.environ["DATA"] + "/la1-small.nrrd")
  img.resample([0.9, 3.14159, 2.5])

  compareImg = Image(os.environ["DATA"] + "/resample4.nrrd")

  return img.compare(compareImg)

success &= utils.test(resampleTest4)

def resampleTest5():
  img = Image(os.environ["DATA"] + "/la1-small.nrrd")
  transform = np.identity(4)
  img.resample(transform, [-100, -63.2, -42.9], [32, 32, 4], [6.25, 6.25, 13.75], img.coordsys())

  compareImg = Image(os.environ["DATA"] + "/resample5.nrrd")

  return img.compare(compareImg)

success &= utils.test(resampleTest5)

sys.exit(not success)
