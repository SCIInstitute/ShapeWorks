import os
import sys
from shapeworks import *

def scalefailTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.scale()

  compareImg = Image(os.environ["DATA"] + "/scalefail.nrrd")

  return img.compare(compareImg)

val = scalefailTest1()

if val is False:
  sys.exit(1)

def scalefailTest2():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.scale([1.0, 1.0, 1.0])

  compareImg = Image(os.environ["DATA"] + "/scalefail.nrrd")

  return img.compare(compareImg)

val = scalefailTest2()

if val is False:
  sys.exit(1)