import os
import sys
from shapeworks import *

success = True

def cropTest1():
  img = Image(os.environ["DATA"] + "/seg.ellipsoid_1.nrrd")

  region = img.logicalBoundingBox().pad(-16)

  region.min[0] = 7
  region.max[0] = 42

  img.crop(img.logicalToPhysical(region))

  compareImg = Image(os.environ["DATA"] + "/crop_baseline.nrrd")

  return img.compare(compareImg)

success &= utils.test(cropTest1)

def cropTest2():
  img1 = Image(os.environ["DATA"] + "/many/seg.ellipsoid_1.nrrd")
  img2 = Image(os.environ["DATA"] + "/many/seg.ellipsoid_3.nrrd")
  img3 = Image(os.environ["DATA"] + "/many/seg.ellipsoid_4.nrrd")

  region = ImageUtils.boundingBox([img1, img2, img3])
  img1.crop(region)

  compareImg = Image(os.environ["DATA"] + "/cropped2.nrrd")

  return img1.compare(compareImg)

success &= utils.test(cropTest2)

def cropTest3():
  img = Image(os.environ["DATA"] + "/seg.ellipsoid_1.nrrd")
  img.crop(img.physicalBoundingBox(0.5))
  img.write(os.environ["DATA"] + "/ellipsoid_crop.nrrd")
  img.resample(1.0)

  cropped = Image(os.environ["DATA"] + "/ellipsoid_crop.nrrd")
  cropped.resample(1.0)

  return img.compare(cropped)

success &= utils.test(cropTest3)

sys.exit(not success)
