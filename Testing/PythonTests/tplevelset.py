import os
import sys
from shapeworks import *

def tplevelsetTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  featureImg = Image(os.environ["DATA"] + "/curvature1.nrrd")

  img.applyTPLevelSetFilter(featureImg, 10.0)

  compareImg = Image(os.environ["DATA"] + "/tplevelset1.nrrd")

  return img.compare(compareImg)

try:
  tplevelsetTest1()
except ValueError:
  print("tplevelsetTest1 failed")
  sys.exit(1)

def tplevelsetTest2():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  featureImg = Image(os.environ["DATA"] + "/curvature1.nrrd")

  img.applyTPLevelSetFilter(featureImg, -10.0)

  compareImg = Image(os.environ["DATA"] + "/tplevelset2.nrrd")

  return img.compare(compareImg)

try:
  tplevelsetTest2()
except ValueError:
  print("tplevelsetTest2 failed")
  sys.exit(1)

def tplevelsetfailTest():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.applyTPLevelSetFilter()

  compareImg = Image(os.environ["DATA"] + "/tplevelsetfail.nrrd")

  return img.compare(compareImg)

try:
  tplevelsetfailTest()
  print("tplevelsetfailTest failed. The function requires feautre image.")
  sys.exit(1)
except TypeError:
  sys.exit(0)