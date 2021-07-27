import os
import sys
from shapeworks import *

def computedtTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.computeDT()

  compareImg = Image(os.environ["DATA"] + "/computedt1.nrrd")

  return img.compare(compareImg)

try:
  computedtTest1()
except ValueError:
  print("computedtTest1 failed")
  sys.exit(1)

def computedtTest2():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.computeDT(1.0)

  compareImg = Image(os.environ["DATA"] + "/computedt2.nrrd")

  return img.compare(compareImg)

try:
  computedtTest2()
except ValueError:
  print("computedtTest2 failed")
  sys.exit(1)

def computedtTest3():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.computeDT(-1.0)

  compareImg = Image(os.environ["DATA"] + "/computedt3.nrrd")

  return img.compare(compareImg)

try:
  computedtTest3()
except ValueError:
  print("computedtTest3 failed")
  sys.exit(1)
