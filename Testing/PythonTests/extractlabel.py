import os
import sys
from shapeworks import *

def extractlabelTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.extractLabel()

  compareImg = Image(os.environ["DATA"] + "/extractlabel1.nrrd")

  return img.compare(compareImg)

try:
  extractlabelTest1()
except ValueError:
  print("extractlabelTest1 failed")
  sys.exit(1)

def extractlabelTest2():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.extractLabel(2.0)

  compareImg = Image(os.environ["DATA"] + "/extractlabel2.nrrd")

  return img.compare(compareImg)

try:
  extractlabelTest2()
except ValueError:
  print("extractlabelTest2 failed")
  sys.exit(1)

def extractlabelTest3():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.extractLabel(-2.0)

  compareImg = Image(os.environ["DATA"] + "/extractlabel3.nrrd")

  return img.compare(compareImg)

try:
  extractlabelTest3()
except ValueError:
  print("extractlabelTest3 failed")
  sys.exit(1)

def extractlabelTest4():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.extractLabel(0)

  compareImg = Image(os.environ["DATA"] + "/extractlabel4.nrrd")

  return img.compare(compareImg)

try:
  extractlabelTest4()
except ValueError:
  print("extractlabelTest4 failed")
  sys.exit(1)
