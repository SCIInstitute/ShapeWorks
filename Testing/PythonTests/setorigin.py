import os
import sys
from shapeworks import *

def setoriginTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.setOrigin()

  compareImg = Image(os.environ["DATA"] + "/setorigin1.nrrd")

  return img.compare(compareImg)

try:
  setoriginTest1()
except ValueError:
  print("setoriginTest1 failed")
  sys.exit(1)

def setoriginTest2():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.setOrigin([1, 0, 0])

  compareImg = Image(os.environ["DATA"] + "/setorigin2.nrrd")

  return img.compare(compareImg)

try:
  setoriginTest2()
except ValueError:
  print("setoriginTest2 failed")
  sys.exit(1)

def setoriginTest3():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.setOrigin([-1, 0, 0])

  compareImg = Image(os.environ["DATA"] + "/setorigin3.nrrd")

  return img.compare(compareImg)

try:
  setoriginTest3()
except ValueError:
  print("setoriginTest3 failed")
  sys.exit(1)

def setoriginTest4():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.setOrigin([1, 1, 1])

  compareImg = Image(os.environ["DATA"] + "/setorigin4.nrrd")

  return img.compare(compareImg)

try:
  setoriginTest4()
except ValueError:
  print("setoriginTest4 failed")
  sys.exit(1)
