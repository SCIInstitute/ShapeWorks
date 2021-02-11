import os
import sys
import shapeworks as sw

def compareTest1():
  img = sw.Image(os.environ["DATA"] + "/la-bin.nrrd")
  compareImg = sw.Image(os.environ["DATA"] + "/la-bin.nrrd")

  return img.compare(compareImg)

val = compareTest1()

if val is False:
  sys.exit(1)

def compareTest2():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  compareImg = sw.Image(os.environ["DATA"] + "/1x2x2-diff.nrrd")

  return img.compare(compareImg, tolerance=1.0)

val = compareTest2()

if val is False:
  sys.exit(1)