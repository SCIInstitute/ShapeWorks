import os
import sys
from shapeworks import *

def centerofmassTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  xform = img.createTransform()
  img.applyTransform(xform)

  compareImg = Image(os.environ["DATA"] + "/centerofmass1.nrrd")

  return img.compare(compareImg)

utils.test(centerofmassTest1)

def centerofmassTest2():
  img = Image(os.environ["DATA"] + "/la-bin.nrrd")
  xform = img.createTransform()
  img.applyTransform(xform)

  compareImg = Image(os.environ["DATA"] + "/centerofmass2.nrrd")

  return img.compare(compareImg)

utils.test(centerofmassTest2)
