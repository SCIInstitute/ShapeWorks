import os
import sys
from shapeworks import *

def tplevelsetTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  featureImg = Image(os.environ["DATA"] + "/curvature1.nrrd")

  img.applyTPLevelSetFilter(featureImg, 10.0)

  compareImg = Image(os.environ["DATA"] + "/tplevelset1.nrrd")

  return img.compare(compareImg)

val = tplevelsetTest1()

if val is False:
  print("tplevelsetTest1 failed")
  sys.exit(1)

def tplevelsetTest2():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  featureImg = Image(os.environ["DATA"] + "/curvature1.nrrd")

  img.applyTPLevelSetFilter(featureImg, -10.0)

  compareImg = Image(os.environ["DATA"] + "/tplevelset2.nrrd")

  return img.compare(compareImg)

val = tplevelsetTest2()

if val is False:
  print("tplevelsetTest2 failed")
  sys.exit(1)

def tplevelsetfailTest():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.applyTPLevelSetFilter()

  compareImg = Image(os.environ["DATA"] + "/tplevelsetfail.nrrd")

  return img.compare(compareImg)

try:
  val = tplevelsetfailTest()
  sys.exit(1)
except TypeError:
  sys.exit(0)