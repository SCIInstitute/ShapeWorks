import os
import sys
from shapeworks import *

def resizeTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.resize([0, 0, 0])

  compareImg = Image(os.environ["DATA"] + "/1x2x2.nrrd")

  return img.compare(compareImg)

val = resizeTest1()

if val is False:
  sys.exit(1)

def resizeTest2():
  img = Image(os.environ["DATA"] + "/la1-small.nrrd")
  img.resize([2, 0, 0])

  compareImg = Image(os.environ["DATA"] + "/resize2.nrrd")

  return img.compare(compareImg)

val = resizeTest2()

if val is False:
  sys.exit(1)

def resizeTest3():
  img = Image(os.environ["DATA"] + "/la1-small.nrrd")
  img.resize([96, 96, 12])

  compareImg = Image(os.environ["DATA"] + "/resize3.nrrd")

  return img.compare(compareImg)

val = resizeTest2()

if val is False:
  sys.exit(1)

def resizeTest4():
  img = Image(os.environ["DATA"] + "/la1-small.nrrd")
  img.resize([24, 120, 6])

  compareImg = Image(os.environ["DATA"] + "/resize4.nrrd")

  return img.compare(compareImg)

val = resizeTest4()

if val is False:
  sys.exit(1)