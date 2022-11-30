import os
import sys
from shapeworks import *

success = True

def sigmoidTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.applySigmoidFilter()

  compareImg = Image(os.environ["DATA"] + "/sigmoid1.nrrd")

  return img.compare(compareImg)

success &= utils.test(sigmoidTest1)

def sigmoidTest2():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.applySigmoidFilter(10.5, 10.5)

  compareImg = Image(os.environ["DATA"] + "/sigmoid2.nrrd")

  return img.compare(compareImg)

success &= utils.test(sigmoidTest2)

def sigmoidTest3():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.applySigmoidFilter(-10.5)

  compareImg = Image(os.environ["DATA"] + "/sigmoid3.nrrd")

  return img.compare(compareImg)

success &= utils.test(sigmoidTest3)

def sigmoidTest4():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.applySigmoidFilter(beta=-10.5)

  compareImg = Image(os.environ["DATA"] + "/sigmoid4.nrrd")

  return img.compare(compareImg)

success &= utils.test(sigmoidTest4)

sys.exit(not success)
