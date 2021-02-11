import os
import sys
import shapeworks as sw

def binarizeTest1():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.binarize()

  compareImg = sw.Image(os.environ["DATA"] + "/binarize1.nrrd")

  return img.compare(compareImg)

val = binarizeTest1()

if val is False:
  sys.exit(1)

def binarizeTest2():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.binarize(minVal=-0.1)

  compareImg = sw.Image(os.environ["DATA"] + "/binarize2.nrrd")

  return img.compare(compareImg)

val = binarizeTest2()

if val is False:
  sys.exit(1)