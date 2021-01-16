import os
import sys
from shapeworks import *

def setSpacingTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.setSpacing([1.0, 1.0, 2.0])
  return img.spacing() == [1.0, 1.0, 2.0];

val = setSpacingTest1()

if val is False:
  sys.exit(1)

def setSpacingTest2():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.setSpacing([1.0, 1.0, 2.0]).setSpacing([0.5, 1.0, 1.0]) # chain calls
  return img.spacing() == [0.5, 1.0, 1.0];

val = setSpacingTest2()

if val is False:
  sys.exit(1)
