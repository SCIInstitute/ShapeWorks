import os
import sys
from shapeworks import *

success = True

def padTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.pad(0, 0.0)

  compareImg = Image(os.environ["DATA"] + "/pad1.nrrd")

  return img.compare(compareImg)

success &= utils.test(padTest1)

def padTest2():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.pad(-1, 0.0)

  compareImg = Image(os.environ["DATA"] + "/pad2.nrrd")

  return img.compare(compareImg)

success &= utils.test(padTest2)

def padTest3():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.pad(0, 1.0)

  compareImg = Image(os.environ["DATA"] + "/pad3.nrrd")

  return img.compare(compareImg)

success &= utils.test(padTest3)

def padTest4():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.pad(100, 25, 1, 10)

  compareImg = Image(os.environ["DATA"] + "/pad4.nrrd")

  return img.compare(compareImg)

success &= utils.test(padTest4)

def padTest5():
  img = Image(os.environ["DATA"] + "/femurImage.nrrd")
  img.pad(img.logicalBoundingBox(), 10)

  compareImg = Image(os.environ["DATA"] + "/femurImage.nrrd")

  return img.compare(compareImg)

success &= utils.test(padTest5)

def padfailTest():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.pad()   # should fail because no version without parameters exists

success &= utils.expectException(padfailTest, TypeError)

sys.exit(not success)
