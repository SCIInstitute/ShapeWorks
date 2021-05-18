import os
import sys
from shapeworks import *

def cropTest1():
  img = Image(os.environ["DATA"] + "/seg.ellipsoid_1.nrrd")

  region = img.physicalBoundingBox()
  
  # [] fixme!
  # region.min[0] = 7
  # region.max[0] = 42
  region.min = [7, region.min[1], region.min[2]]
  region.max = [42, region.max[1], region.max[2]]

  img.crop(region, -16)

  compareImg = Image(os.environ["DATA"] + "/crop_baseline.nrrd")

  return img.compare(compareImg)

val = cropTest1()

if val is False:
  print("cropTest1 failed")
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
  print("cropTest2 failed")
  sys.exit(1)
