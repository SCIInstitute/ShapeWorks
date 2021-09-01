import os
import sys
from shapeworks import *

def blurTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.gaussianBlur()

  compareImg = Image(os.environ["DATA"] + "/blur1.nrrd")

  return img.compare(compareImg)

utils.test(blurTest1)

def blurTest2():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.gaussianBlur(1.0)

  compareImg = Image(os.environ["DATA"] + "/blur2.nrrd")

  return img.compare(compareImg)

utils.test(blurTest2)

def blurTest3():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.gaussianBlur(-1.0)

  compareImg = Image(os.environ["DATA"] + "/blur3.nrrd")

  return img.compare(compareImg)

utils.test(blurTest3)
