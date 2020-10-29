import os
import sys
from shapeworks import *

def padfailTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.pad()

  compareImg = Image(os.environ["DATA"] + "/padfail.nrrd")

  return img.compare(compareImg)

val = padfailTest1()

if val is False:
  sys.exit(1)

def padfailTest2():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.pad(0)

  compareImg = Image(os.environ["DATA"] + "/padfail.nrrd")

  return img.compare(compareImg)

val = padfailTest2()

if val is False:
  sys.exit(1)