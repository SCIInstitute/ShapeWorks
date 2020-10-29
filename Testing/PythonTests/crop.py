import os
import sys
from shapeworks import *

def cropTest():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")

  region = Region(img.dims())
  region.min[0] = 25
  region.max[0] = 50
  region.min[1] = 1
  region.max[1] = 40
  region.min[2] = 12
  region.max[2] = 23
  
  img.crop(region)

  compareImg = Image(os.environ["DATA"] + "/cropped.nrrd")

  return img.compare(compareImg)

val = cropTest()

if val is False:
  sys.exit(1)