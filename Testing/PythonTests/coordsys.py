import os
import sys
from numpy import identity, array_equal
from shapeworks import *

success = True

def getCoordsysTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  return array_equal(img.coordsys(), identity(3))

success &= utils.test(getCoordsysTest1)

def setCoordsysTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  csys = [[1, 0, 0], [0, 1, 0], [0, 0, -2]]
  img.setCoordsys(csys)
  return array_equal(img.coordsys(), csys)

success &= utils.test(setCoordsysTest1)

sys.exit(not success)
