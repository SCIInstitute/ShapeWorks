import os
import sys
from shapeworks import *

def compareTest1():
  img = Image(os.environ["DATA"] + "/la-bin.nrrd")
  compareImg = Image(os.environ["DATA"] + "/la-bin.nrrd")

  return img.compare(compareImg)

utils.test(compareTest1)

def compareTest2():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  compareImg = Image(os.environ["DATA"] + "/1x2x2-diff.nrrd")

  return img.compare(compareImg, tolerance=1.0)

utils.test(compareTest2)

def comparefailTest():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")

  compareImg = Image(os.environ["DATA"] + "/la-bin.nrrd")

  return img.compare(compareImg)

utils.test(comparefailTest, True)
