import os
import sys
from shapeworks import *

def computedtTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.computeDT()

  compareImg = Image(os.environ["DATA"] + "/computedt1.nrrd")

  return img.compare(compareImg)

utils.test(computedtTest1)

def computedtTest2():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.computeDT(1.0)

  compareImg = Image(os.environ["DATA"] + "/computedt2.nrrd")

  return img.compare(compareImg)

utils.test(computedtTest2)

def computedtTest3():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.computeDT(-1.0)

  compareImg = Image(os.environ["DATA"] + "/computedt3.nrrd")

  return img.compare(compareImg)

utils.test(computedtTest3)
