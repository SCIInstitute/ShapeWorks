import os
import sys
import shapeworks as sw

def curvatureTest1():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.applyCurvatureFilter()

  compareImg = sw.Image(os.environ["DATA"] + "/curvature1.nrrd")

  return img.compare(compareImg)

val = curvatureTest1()

if val is False:
  sys.exit(1)

def curvatureTest2():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.applyCurvatureFilter(5)

  compareImg = sw.Image(os.environ["DATA"] + "/curvature2.nrrd")

  return img.compare(compareImg)

val = curvatureTest2()

if val is False:
  sys.exit(1)