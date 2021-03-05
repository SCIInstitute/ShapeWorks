import os
import sys
from shapeworks import *

def setspacingTest1():
  img = Image(os.environ["DATA"] + "/la1-small.nrrd")
  img.setSpacing([2,2,2])

  compareImg = Image(os.environ["DATA"] + "/spacing2.nrrd")

  return img.spacing() == [2.0, 2.0, 2.0] and img.compare(compareImg)

val = setspacingTest1()

if val is False:
  sys.exit(1)

def setSpacingTest2():
  img = Image(os.environ["DATA"] + "/la1-small.nrrd")
  img.setSpacing([1.0, 1.0, 2.0]).setSpacing([0.5, 1.0, 1.0]) # chain calls
  return img.spacing() == [0.5, 1.0, 1.0];

  compareImg = Image(os.environ["DATA"] + "/spacing1.nrrd")

  return img.compare(compareImg)

val = setspacingTest2()

if val is False:
  sys.exit(1)
