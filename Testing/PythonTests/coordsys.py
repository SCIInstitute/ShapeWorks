import os
import sys
from numpy import identity, array_equal
from shapeworks import *

def getCoordsysTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  return array_equal(img.coordsys(), identity(3))

utils.test(getCoordsysTest1)

def setCoordsysTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  csys = [[1, 0, 0], [0, 1, 0], [0, 0, -2]]
  img.setCoordsys(csys)
  return array_equal(img.coordsys(), csys)

utils.test(setCoordsysTest1)
