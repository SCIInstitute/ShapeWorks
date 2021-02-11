import os
import sys
import shapeworks as sw

def tplevelsetTest1():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  featureImg = sw.Image(os.environ["DATA"] + "/curvature1.nrrd")

  img.applyTPLevelSetFilter(featureImg, 10.0)

  compareImg = sw.Image(os.environ["DATA"] + "/tplevelset1.nrrd")

  return img.compare(compareImg)

val = tplevelsetTest1()

if val is False:
  sys.exit(1)

def tplevelsetTest2():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  featureImg = sw.Image(os.environ["DATA"] + "/curvature1.nrrd")

  img.applyTPLevelSetFilter(featureImg, -10.0)

  compareImg = sw.Image(os.environ["DATA"] + "/tplevelset2.nrrd")

  return img.compare(compareImg)

val = tplevelsetTest2()

if val is False:
  sys.exit(1)