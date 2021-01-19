import os
import sys
from shapeworks import *

def divTest1():
  img = Image(os.environ["DATA"] + "/la-bin.nrrd")
  img /= 3.14

  compareImg = Image(os.environ["DATA"] + "/baseline_divide.nrrd")

  return img.compare(compareImg)

val = divTest1()

if val is False:
  sys.exit(1)