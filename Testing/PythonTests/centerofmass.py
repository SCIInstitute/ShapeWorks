import os
import sys
from shapeworks import *

def centerofmassTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  xform = img.createTransform()
  img.applyTransform(xform)

  compareImg = Image(os.environ["DATA"] + "/centerofmass1.nrrd")

  return img.compare(compareImg)

val = centerofmassTest1()

if val is False:
  print("centerofmassTest1 failed")
  sys.exit(1)

def centerofmassTest2():
  img = Image(os.environ["DATA"] + "/la-bin.nrrd")
  xform = img.createTransform()
  img.applyTransform(xform)

  compareImg = Image(os.environ["DATA"] + "/centerofmass2.nrrd")

  return img.compare(compareImg)

val = centerofmassTest2()

if val is False:
  print("centerofmassTest2 failed")
  sys.exit(1)
