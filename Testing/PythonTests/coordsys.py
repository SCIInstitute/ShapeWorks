import os
import sys
from numpy import identity
from shapeworks import *

def getCoordsysTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  return img.getCoordsys() == identity(3)

utils.test(getCoordsysTest1)

def setCoordsysTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  csys = [[1, 0, 0], [0, 1, 0], [0, 0, -2]]
  img.setCoordsys(csys)
  return img.getCoordsys() == csys

utils.test(setCoordsysTest1)
