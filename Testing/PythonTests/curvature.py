import os
import sys
from shapeworks import *

def curvatureTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.applyCurvatureFilter()

  compareImg = Image(os.environ["DATA"] + "/curvature1.nrrd")

  return img.compare(compareImg)

val = curvatureTest1()

if val is False:
  sys.exit(1)

def curvatureTest2():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.applyCurvatureFilter(5)

  compareImg = Image(os.environ["DATA"] + "/curvature2.nrrd")

  return img.compare(compareImg)

val = curvatureTest2()

if val is False:
  sys.exit(1)