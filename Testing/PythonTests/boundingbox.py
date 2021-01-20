import os
import sys
import glob
from shapeworks import *

def boundingboxTest1():
  filenames = os.environ["DATA"] + "/many/"
  region = ImageUtils.boundingBox(glob.glob(filenames + "/*.nrrd"))

  return region.valid()

val = boundingboxTest1()

if val is False:
  sys.exit(1)

def boundingboxTest2():
  img1 = Image(os.environ["DATA"] + "/many/seg.ellipsoid_1.nrrd")
  img2 = Image(os.environ["DATA"] + "/many/seg.ellipsoid_3.nrrd")
  img3 = Image(os.environ["DATA"] + "/many/seg.ellipsoid_4.nrrd")

  imgList = []
  imgList.append(img1)
  imgList.append(img2)
  imgList.append(img3)

  region = ImageUtils.boundingBox(imgList)

  return region.valid()

val = boundingboxTest2()

if val is False:
  sys.exit(1)