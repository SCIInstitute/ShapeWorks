import os
import sys
import numpy as np
from shapeworks import *

def setspacingTest1():
  img = Image(os.environ["DATA"] + "/la1-small.nrrd")
  img.setSpacing([2,2,2])

  compareImg = Image(os.environ["DATA"] + "/spacing2.nrrd")

  return np.array_equal(img.spacing(), [2.0, 2.0, 2.0]) and img.compare(compareImg)

val = setspacingTest1()

if val is False:
  print("setspacingTest1 failed")
  sys.exit(1)

def setSpacingTest2():
  img = Image(os.environ["DATA"] + "/la1-small.nrrd")
  img.setSpacing([1.0, 1.0, 2.0]).setSpacing([0.5, 1.0, 1.0]) # chain calls
  equal = np.array_equal(img.spacing(), [0.5, 1.0, 1.0]) 
  img.setSpacing([1.0, 1.0, 1.0])   # change spacing back

  compareImg = Image(os.environ["DATA"] + "/spacing1.nrrd")

  return np.array_equal(img.spacing(), [1.0, 1.0, 1.0]) and img.compare(compareImg)

val = setSpacingTest2()

if val is False:
  print("setspacingTest2 failed")
  sys.exit(1)
