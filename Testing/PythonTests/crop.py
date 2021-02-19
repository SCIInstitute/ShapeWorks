import os
import sys
from shapeworks import *

def cropTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")

  region = Region(img.dims())
  region.min[0] = 25
  region.max[0] = 50
  region.min[1] = 1
  region.max[1] = 40
  region.min[2] = 12
  region.max[2] = 23
  
  img.crop(region)

  compareImg = Image(os.environ["DATA"] + "/cropped1.nrrd")

  return img.compare(compareImg)

val = cropTest1()

if val is False:
  sys.exit(1)

def cropTest2():
  img1 = Image(os.environ["DATA"] + "/many/seg.ellipsoid_1.nrrd")
  img2 = Image(os.environ["DATA"] + "/many/seg.ellipsoid_3.nrrd")
  img3 = Image(os.environ["DATA"] + "/many/seg.ellipsoid_4.nrrd")

  region = ImageUtils.boundingBox([img1, img2, img3])
  img1.crop(region)

  compareImg = Image(os.environ["DATA"] + "/cropped2.nrrd")

  return img1.compare(compareImg)

val = cropTest2()

if val is False:
  sys.exit(1)
