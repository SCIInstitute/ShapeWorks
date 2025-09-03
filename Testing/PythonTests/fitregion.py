import os
import sys
from shapeworks import *

success = True

def fitRegion1():
  img = Image(os.environ["DATA"] + "/seg.ellipsoid_1.nrrd")

  region = img.logicalBoundingBox().pad(16)

  region.min[0] = 7
  region.max[0] = 42

  img.fitRegion(img.logicalToPhysical(region))

#  img.write(os.environ["DATA"] + "/fitregion1_baseline.nrrd")
  compareImg = Image(os.environ["DATA"] + "/fitregion1_baseline.nrrd")

  return img.compare(compareImg)

success &= utils.test(fitRegion1)


def fitRegion2():
  img = Image(os.environ["DATA"] + "/seg.ellipsoid_1.nrrd")

  region = img.logicalBoundingBox().pad(16)

  region.min[0] = 7
  region.max[0] = 42

  img.fitRegion(img.logicalToPhysical(region), -2000)

#  img.write(os.environ["DATA"] + "/fitregion2_baseline.nrrd")
  compareImg = Image(os.environ["DATA"] + "/fitregion2_baseline.nrrd")

  return img.compare(compareImg)

success &= utils.test(fitRegion2)
