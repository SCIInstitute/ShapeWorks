import os
import sys
from shapeworks import *

def subTest1():
  img1 = Image(os.environ["DATA"] + "/img1.nrrd")
  img2 = Image(os.environ["DATA"] + "/img2.nrrd")
  img = img1 - img2

  compareImg = Image(os.environ["DATA"] + "/img_diff.nrrd")

  return img.compare(compareImg)

val = subTest1()

if val is False:
  sys.exit(1)

def subTest2():
  img = Image(os.environ["DATA"] + "/la-bin.nrrd")
  img -= 3.14

  compareImg = Image(os.environ["DATA"] + "/la-bin-minus-pi.nrrd")

  return img.compare(compareImg)

val = subTest2()

if val is False:
  sys.exit(1)

def subTest3():
  img = Image(os.environ["DATA"] + "/la-bin.nrrd")
  img -= img
  img -= 3.14

  compareImg = Image(os.environ["DATA"] + "/la-bin-doubled-minus-pi.nrrd")

  return img.compare(compareImg)

val = subTest3()

if val is False:
  sys.exit(1)