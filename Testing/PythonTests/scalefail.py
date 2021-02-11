import os
import sys
import shapeworks as sw

def scalefailTest1():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.scale()

  compareImg = sw.Image(os.environ["DATA"] + "/scalefail.nrrd")

  return img.compare(compareImg)

val = scalefailTest1()

if val is False:
  sys.exit(1)

def scalefailTest2():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.scale([1.0, 1.0, 1.0])

  compareImg = sw.Image(os.environ["DATA"] + "/scalefail.nrrd")

  return img.compare(compareImg)

val = scalefailTest2()

if val is False:
  sys.exit(1)