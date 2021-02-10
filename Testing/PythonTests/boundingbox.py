import os
import sys
import glob
import shapeworks as sw

def boundingboxTest1():
  filenames = os.environ["DATA"] + "/many/"
  region = sw.ImageUtils.boundingBox(glob.glob(filenames + "/*.nrrd"))

  return region.valid()

val = boundingboxTest1()

if val is False:
  sys.exit(1)

def boundingboxTest2():
  img1 = sw.Image(os.environ["DATA"] + "/many/seg.ellipsoid_1.nrrd")
  img2 = sw.Image(os.environ["DATA"] + "/many/seg.ellipsoid_3.nrrd")
  img3 = sw.Image(os.environ["DATA"] + "/many/seg.ellipsoid_4.nrrd")

  imgList = []
  imgList.append(img1)
  imgList.append(img2)
  imgList.append(img3)

  region = sw.ImageUtils.boundingBox(imgList)

  return region.valid()

val = boundingboxTest2()

if val is False:
  sys.exit(1)