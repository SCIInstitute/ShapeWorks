import os
import sys
from shapeworks import *

def negateTest():
  img = Image(os.environ["DATA"] + "/la-bin.nrrd")
  img = -img

  compareImg = Image(os.environ["DATA"] + "/la-bin-negative.nrrd")

  return img.compare(compareImg)

try:
  negateTest()
except ValueError:
  print("negateTest failed")
  sys.exit(1)
