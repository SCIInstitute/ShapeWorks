import os
import sys
from shapeworks import *

def binarizeTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.binarize()

  compareImg = Image(os.environ["DATA"] + "/binarize1.nrrd")

  return img.compare(compareImg)

try:
  binarizeTest1()
except ValueError:
  print("binarizeTest1 failed")
  sys.exit(1)

def binarizeTest2():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.binarize(minVal=-0.1)

  compareImg = Image(os.environ["DATA"] + "/binarize2.nrrd")

  return img.compare(compareImg)

try:
  binarizeTest2()
except ValueError:
  print("binarizeTest2 failed")
  sys.exit(1)
