import os
import sys
from shapeworks import *

def resizeTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.resize([0, 0, 0])

  compareImg = Image(os.environ["DATA"] + "/1x2x2.nrrd")

  return img.compare(compareImg)

try:
  resizeTest1()
except ValueError:
  print("resizeeTest1 failed")
  sys.exit(1)

def resizeTest2():
  img = Image(os.environ["DATA"] + "/la1-small.nrrd")
  img.resize([2, 0, 0])

  compareImg = Image(os.environ["DATA"] + "/resize2.nrrd")

  return img.compare(compareImg)

try:
  resizeTest2()
except ValueError:
  print("resizeeTest2 failed")
  sys.exit(1)

def resizeTest3():
  img = Image(os.environ["DATA"] + "/la1-small.nrrd")
  img.resize([96, 96, 12])

  compareImg = Image(os.environ["DATA"] + "/resize3.nrrd")

  return img.compare(compareImg)

try:
  resizeTest3()
except ValueError:
  print("resizeTest3 failed")
  sys.exit(1)

def resizeTest4():
  img = Image(os.environ["DATA"] + "/la1-small.nrrd")
  img.resize([24, 120, 6])

  compareImg = Image(os.environ["DATA"] + "/resize4.nrrd")

  return img.compare(compareImg)

try:
  resizeTest4()
except ValueError:
  print("resizeeTest4 failed")
  sys.exit(1)

def resizefailTest():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.resize()

  compareImg = Image(os.environ["DATA"] + "/resizefail.nrrd")

  return img.compare(compareImg)

try:
  resizefailTest()
  print("resizefailTest failed. There should be no default version of this function.")
  sys.exit(1)
except TypeError:
  sys.exit(0)
