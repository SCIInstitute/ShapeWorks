import os
import sys
import numpy as np
from shapeworks import *

def setspacingTest1():
  img = Image(os.environ["DATA"] + "/la1-small.nrrd")
  img.setSpacing([2,2,2])

  compareImg = Image(os.environ["DATA"] + "/spacing2.nrrd")

  return np.array_equal(img.spacing(), [2.0, 2.0, 2.0]) and img.compare(compareImg)

utils.test(setspacingTest1)

def setspacingTest2():
  img = Image(os.environ["DATA"] + "/la1-small.nrrd")
  img.setSpacing([1.0, 1.0, 2.0]).setSpacing([0.5, 1.0, 1.0]) # chain calls
  equal = np.array_equal(img.spacing(), [0.5, 1.0, 1.0]) 
  img.setSpacing([1.0, 1.0, 1.0])   # change spacing back

  compareImg = Image(os.environ["DATA"] + "/spacing1.nrrd")

  return np.array_equal(img.spacing(), [1.0, 1.0, 1.0]) and img.compare(compareImg)

utils.test(setspacingTest2)

def setspacingfailTest():
  img = Image(os.environ["DATA"] + "/la1-small.nrrd")
  img.setSpacing([0,0,0])

  compareImg = Image(os.environ["DATA"] + "/spacing1.nrrd")

  return img.compare(compareImg)

utils.expectException(setspacingfailTest, ValueError)
