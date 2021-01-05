import os
import sys
from shapeworks import *

def clipTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.clip([0, 20, 0], [1, 55, 0], [1, 75, 0], 0.0)

  compareImg = Image(os.environ["DATA"] + "/clip1.nrrd")

  return img.compare(compareImg)

val = clipTest1()

if val is False:
  sys.exit(1)

def clipTest2():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.clip([0, 20, 0], [1, 55, 0], [1, 75, 0], 3.14)

  compareImg = Image(os.environ["DATA"] + "/clip2.nrrd")

  return img.compare(compareImg)

val = clipTest2()

if val is False:
  sys.exit(1)

def clipTest3():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.clip([0, 20, 0], [1, 55, 1], [1, 75, 1], 0.0)

  compareImg = Image(os.environ["DATA"] + "/clip3.nrrd")

  return img.compare(compareImg)

val = clipTest3()

if val is False:
  sys.exit(1)