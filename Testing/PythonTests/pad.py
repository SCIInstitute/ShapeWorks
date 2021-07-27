import os
import sys
from shapeworks import *

def padTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.pad(0, 0.0)

  compareImg = Image(os.environ["DATA"] + "/pad1.nrrd")

  return img.compare(compareImg)

try:
  padTest1()
except ValueError:
  print("padTest1 failed")
  sys.exit(1)

def padTest2():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.pad(-1, 0.0)

  compareImg = Image(os.environ["DATA"] + "/pad2.nrrd")

  return img.compare(compareImg)

try:
  padTest2()
except ValueError:
  print("padTest2 failed")
  sys.exit(1)

def padTest3():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.pad(0, 1.0)

  compareImg = Image(os.environ["DATA"] + "/pad3.nrrd")

  return img.compare(compareImg)

try:
  padTest3()
except ValueError:
  print("padTest3 failed")
  sys.exit(1)

def padTest4():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.pad(100, 25, 1, 10)

  compareImg = Image(os.environ["DATA"] + "/pad4.nrrd")

  return img.compare(compareImg)

try:
  padTest4()
except ValueError:
  print("padTest4 failed")
  sys.exit(1)

def padTest5():
  img = Image(os.environ["DATA"] + "/femurImage.nrrd")
  img.pad(img.logicalBoundingBox(), 10)

  compareImg = Image(os.environ["DATA"] + "/femurImage.nrrd")

  return img.compare(compareImg)

try:
  padTest5()
except ValueError:
  print("padTest5 failed")
  sys.exit(1)

def padfailTest():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.pad()   # should fail because no version without parameters exists

try:
	padfailTest()
	print("padfailTest failed. There should be no default version of this function.")
	sys.exit(1)
except TypeError:
  sys.exit(0)
