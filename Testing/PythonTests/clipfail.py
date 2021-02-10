import os
import sys
import shapeworks as sw

def clipfailTest1():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.clip([1, 0.0, 0.0], [0.0, 0.0, 0.0], [0.0, 0.0, 0.0], 0.0)

  compareImg = sw.Image(os.environ["DATA"] + "/clipfail.nrrd")

  return img.compare(compareImg)

val = clipfailTest1()

if val is False:
  sys.exit(1)

def clipfailTest2():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.clip([0.0, 0.0, 0.0], [0.0, 0.0, 0.0], [0.0, 0.0, 0.0], 0.0)

  compareImg = sw.Image(os.environ["DATA"] + "/clipfail.nrrd")

  return img.compare(compareImg)

val = clipfailTest2()

if val is False:
  sys.exit(1)