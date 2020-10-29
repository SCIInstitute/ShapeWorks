import os
import sys
from shapeworks import *

def addTest1():
  img = Image(os.environ["DATA"] + "/la-bin.nrrd")
  img += img

  compareImg = Image(os.environ["DATA"] + "/la-bin-doubled.nrrd")

  return img.compare(compareImg)

val = addTest1()

if val is False:
  sys.exit(1)

def addTest2():
  img = Image(os.environ["DATA"] + "/la-bin.nrrd")
  img += 3.14

  compareImg = Image(os.environ["DATA"] + "/la-bin-plus-pi.nrrd")

  return img.compare(compareImg)

val = addTest2()

if val is False:
  sys.exit(1)

def addTest3():
  img = Image(os.environ["DATA"] + "/la-bin.nrrd")
  img += img
  img += 3.14

  compareImg = Image(os.environ["DATA"] + "/la-bin-doubled-plus-pi.nrrd")

  return img.compare(compareImg)

val = addTest3()

if val is False:
  sys.exit(1)