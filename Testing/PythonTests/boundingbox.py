import os
import sys
import glob
from shapeworks import *

def boundingboxTest():
  filenames = os.environ["DATA"] + "/many/"
  region = ImageUtils.boundingBox(glob.glob(filenames + "/*.nrrd"))

  return region.valid()

val = boundingboxTest()

if val is False:
  sys.exit(1)