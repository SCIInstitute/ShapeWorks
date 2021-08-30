import os
import sys
from shapeworks import *

def subTest1():
  img1 = Image(os.environ["DATA"] + "/img1.nrrd")
  img2 = Image(os.environ["DATA"] + "/img2.nrrd")
  img = img1 - img2

  compareImg = Image(os.environ["DATA"] + "/img_diff.nrrd")

  return img.compare(compareImg)

utils.test(subTest1)

def subTest2():
  img = Image(os.environ["DATA"] + "/la-bin.nrrd")
  img -= 3.14

  compareImg = Image(os.environ["DATA"] + "/la-bin-minus-pi.nrrd")

  return img.compare(compareImg)

utils.test(subTest2)

def subTest3():
  img = Image(os.environ["DATA"] + "/la-bin.nrrd")
  img -= img
  img -= 3.14

  compareImg = Image(os.environ["DATA"] + "/la-bin-doubled-minus-pi.nrrd")

  return img.compare(compareImg)

utils.test(subTest3)

def subfailTest():
  img1 = Image(os.environ["DATA"] + "/la-bin.nrrd")
  img2 = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img = img1 - img2

  compareImg = Image(os.environ["DATA"] + "/subfail.nrrd")

  return img.compare(compareImg)

utils.expectException(subfailTest, ValueError)
