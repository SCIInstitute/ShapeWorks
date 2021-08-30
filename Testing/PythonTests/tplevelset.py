import os
import sys
from shapeworks import *

def tplevelsetTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  featureImg = Image(os.environ["DATA"] + "/curvature1.nrrd")

  img.applyTPLevelSetFilter(featureImg, 10.0)

  compareImg = Image(os.environ["DATA"] + "/tplevelset1.nrrd")

  return img.compare(compareImg)

utils.test(tplevelsetTest1)

def tplevelsetTest2():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  featureImg = Image(os.environ["DATA"] + "/curvature1.nrrd")

  img.applyTPLevelSetFilter(featureImg, -10.0)

  compareImg = Image(os.environ["DATA"] + "/tplevelset2.nrrd")

  return img.compare(compareImg)

utils.test(tplevelsetTest2)

def tplevelsetfailTest():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.applyTPLevelSetFilter()

  compareImg = Image(os.environ["DATA"] + "/tplevelsetfail.nrrd")

  return img.compare(compareImg)

utils.expectException(tplevelsetfailTest, TypeError)
