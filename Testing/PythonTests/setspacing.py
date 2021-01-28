import os
import sys
from shapeworks import *

def setspacingTest1():
  img = Image(os.environ["DATA"] + "/la1-small.nrrd")
  img.setSpacing([2,2,2])

  compareImg = Image(os.environ["DATA"] + "/spacing2.nrrd")

  return img.compare(compareImg)

val = setspacingTest1()

if val is False:
  sys.exit(1)

def setspacingTest2():
  img = Image(os.environ["DATA"] + "/la1-small.nrrd")
  img.setSpacing()

  compareImg = Image(os.environ["DATA"] + "/spacing1.nrrd")

  return img.compare(compareImg)

val = setspacingTest2()

if val is False:
  sys.exit(1)